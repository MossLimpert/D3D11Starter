#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Mesh.h"
#include "BufferStructs.h" // Assignment 4

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;


//
// FIELDS
//
XMFLOAT4 _color(0.0f, 1.0f, 0.0f, 1.0f);
XMFLOAT4 _colorTint(1.0f, 1.0f, 1.0f, 1.0f);
XMFLOAT3 _offset(0.0f, 0.0f, 0.0f);
static bool demoActive;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// init fields
	// 
	demoActive = false;


	// IMGUI
	// 
	// initialize itself, platform, and renderer backend
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	ImGui::StyleColorsDark();


	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);
	}

	// Set up Constant Buffers
	//
	{
		D3D11_BUFFER_DESC cbDesc = {};
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = (sizeof(VertexShaderData) + 15) / 16 * 16;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;

		Graphics::Device->CreateBuffer(&cbDesc, 0, vsConstBuff.GetAddressOf());

		// 4 bind constant buffer for drawing
		Graphics::Context->VSSetConstantBuffers(0, 1, vsConstBuff.GetAddressOf());
	}

	
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	
	// imgui cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 periwinkle = XMFLOAT4(0.68f, 0.4f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
	};
	Vertex heartVerts[] = {
		{ XMFLOAT3(-0.6f, 0.4f, 0.0f), blue},
		{ XMFLOAT3(-0.8f, 0.8f, 0.0f), red},
		{ XMFLOAT3(-0.75f, 0.9f, 0.0f), red},
		{ XMFLOAT3(-0.65f, 0.9f, 0.0f), red},
		{ XMFLOAT3(-0.6f, 0.85f, 0.0f), periwinkle},
		{ XMFLOAT3(-0.55f, 0.9f, 0.0f), periwinkle},
		{ XMFLOAT3(-0.45f, 0.9f, 0.0f), periwinkle},
		{ XMFLOAT3(-0.4f, 0.8f, 0.0f), periwinkle}
	};
	Vertex bunnyVerts[] = {
		{ XMFLOAT3(0.2f, 0.5f, 0.0f), blue},
		{ XMFLOAT3(0.25f, 0.7f,0.0f), blue},
		{ XMFLOAT3(0.2f, 0.8f, 0.0f), blue},
		{ XMFLOAT3(0.25f, 0.85f,0.0f), blue},
		{ XMFLOAT3(0.3f, 0.85f, 0.0f), periwinkle},
		{ XMFLOAT3(0.35f, 0.95f, 0.0f), periwinkle},
		{ XMFLOAT3(0.45f, 1.0f, 0.0f), periwinkle},
		{ XMFLOAT3(0.45f, 0.9f, 0.0f), periwinkle},
		{ XMFLOAT3(0.35f, 0.8f, 0.0f), periwinkle},
		{ XMFLOAT3(0.35f, 0.725f, 0.0f), blue},
		{ XMFLOAT3(0.4f, 0.75f, 0.0f), blue},
		{ XMFLOAT3(0.6f, 0.75f, 0.0f), blue},
		{ XMFLOAT3(0.7f, 0.7f, 0.0f), blue},
		{ XMFLOAT3(0.65f, 0.65f, 0.0f), blue},
		{ XMFLOAT3(0.75f, 0.5f, 0.0f), blue},
	};


	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };
	unsigned int heartIndices[] = { 0,1,2,0,2,3,0,3,4,0,4,5,0,5,6,0,6,7 };
	unsigned int bunnyIndices[] = { 0,1,9,9,1,2,9,2,3,9,3,4,9,4,8,8,4,5,8,5,6,8,6,7,0,9,10,0,10,11,0,11,12,0,12,13,0,13,14 };
	// use mesh class!
	// 
	std::shared_ptr<Mesh> triangle = std::make_shared<Mesh>("triangle", vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices));
	std::shared_ptr<Mesh> heart = std::make_shared<Mesh>("heart", heartVerts, ARRAYSIZE(heartVerts), heartIndices, ARRAYSIZE(heartIndices));
	std::shared_ptr<Mesh> bnuy = std::make_shared<Mesh>("bunny", bunnyVerts, ARRAYSIZE(bunnyVerts), bunnyIndices, ARRAYSIZE(bunnyIndices));

	meshes.push_back(triangle);
	meshes.push_back(heart);
	meshes.push_back(bnuy);

}

/// <summary>
/// Updates ImGUI
/// </summary>
void Game::GuiUpdate(float deltaTime)
{
	// give imgui data
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();

	// reset frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// show demo
	//ImGui::ShowDemoWindow();

}

void Game::BuildGui()
{
	// vars
	//XMFLOAT4 color(0.0f, 1.0f, 0.0f, 1.0f);

	ImGui::Begin("Moss Limpert's D3D11 Homework");	// custom window

	ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);	// current framerate
	ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());	// window dimenstions

	ImGui::ColorEdit4("RGBA Color Editor", &_color.x);

	
	//if (ImGui::Checkbox("Show Demo Window", &demoActive))
	//if (demoActive) ImGui::ShowDemoWindow();

	ImGui::DragFloat3("Offset: ", &_offset.x, 0.01f);
	ImGui::ColorEdit4("Color Tint: ", &_colorTint.x);

	if (ImGui::TreeNode("Mesh Information")) {
		for (auto& m : meshes) {
			ImGui::Text("Name: %s", m->GetName()); 
			ImGui::Text("Tris: %d | ", m->GetIndexCount() / 3); ImGui::SameLine();
			ImGui::Text("Verts: %d | ", m->GetVertexCount()); ImGui::SameLine();
			ImGui::Text("Indices: %d", m->GetIndexCount());
		}
		ImGui::TreePop();
	}
	

	//if (ImGui::TreeNode("Assignment 2 Extras")) {
	//	if (ImGui::TreeNode("Basic Tree")) {
	//		for (int i = 0; i < 5; i++) {
	//			if (i == 0) ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	//			if (ImGui::TreeNode((void*)(intptr_t)i, "Child %d", i)) {
	//				ImGui::Text("This is child node number %d", i);
	//				ImGui::TreePop();
	//			}
	//		}
	//		ImGui::TreePop();
	//	}

	//	if (ImGui::TreeNode("List box")) {
	//		const char* items[] = { "dog", "cat", "snake", "lizard", "salamander", "rabbit" };
	//		static int item_current_idx = 0;
	//		if (ImGui::BeginListBox("animals")) {
	//			for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
	//				const bool is_selected = (item_current_idx == n);
	//				if (ImGui::Selectable(items[n], is_selected)) item_current_idx = n;

	//				// set focus
	//				if (is_selected) ImGui::SetItemDefaultFocus();
	//			}
	//			ImGui::EndListBox();
	//		}
	//		ImGui::TreePop();
	//	}

	//	if (ImGui::TreeNode("Table")) {
	//		if (ImGui::BeginTable("Example Table", 3)) {
	//			for (int row = 0; row < 4; row++) {
	//				ImGui::TableNextRow();
	//				for (int column = 0; column < 3; column++) {
	//					ImGui::TableSetColumnIndex(column);
	//					ImGui::Text("Row %d Column %d", row, column);
	//				}
	//			}
	//			ImGui::EndTable();
	//		}
	//		ImGui::TreePop();
	//	}

	//	ImGui::TreePop();
	//}

	


	ImGui::End();
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	
	// update imgui info 
	GuiUpdate(deltaTime);
	BuildGui();
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}



// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		float color[4] = { _color.x, _color.y, _color.z, _color.w };
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		// update constant buffers
		{
			// Assignment 4 constant buffer collect data
			VertexShaderData vsData;
			vsData.colorTint = _colorTint;
			vsData.offset = _offset;

			// Assingment 4 constant buffer map
			D3D11_MAPPED_SUBRESOURCE mappedBuff = {};
			Graphics::Context->Map(
				vsConstBuff.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mappedBuff
			);

			// 4 memcpy
			memcpy(mappedBuff.pData, &vsData, sizeof(vsData));

			// 4 unmap
			Graphics::Context->Unmap(vsConstBuff.Get(), 0);
		}
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	{
		//printf("%i", meshes.size());
		//for (int i = meshes.size() - 1; i > 0; i--) {
		for (auto& m : meshes) {
			m->Draw();
		}
	}

	//
	// IMGUI
	//
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}



