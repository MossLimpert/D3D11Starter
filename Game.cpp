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
#include "BufferStructs.h"	// Assignment 4
#include "Material.h"		// Assignment 7

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;


//
// FIELDS
//
XMFLOAT4 _color(0.5f, 0.0f, 0.5f, 1.0f);
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
	curCamera = 0;
	ambient = DirectX::XMFLOAT3(0.1, 0.1, 0.25);
	InitializeCamera();

	//lights = std::vector<Light>();

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
	CreateMaterials();
	CreateGeometry();
	CreateLights();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
	vertexShader = std::make_shared<SimpleVertexShader>(Graphics::Device,
		Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(Graphics::Device,
		Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	normalsPS = std::make_shared<SimplePixelShader>(Graphics::Device,
		Graphics::Context, FixPath(L"normalPS.cso").c_str());
	uvPS = std::make_shared<SimplePixelShader>(Graphics::Device,
		Graphics::Context, FixPath(L"uvPS.cso").c_str());
	pseudoPS = std::make_shared<SimplePixelShader>(Graphics::Device,
		Graphics::Context, FixPath(L"pseudoPS.cso").c_str());
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// LOAD MODELS
	
	std::shared_ptr<Mesh> sph = std::make_shared<Mesh>("sphere",
		FixPath(L"../../meshes/sphere.obj").c_str()
	);
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>("cube",
		FixPath(L"../../meshes/cube.obj").c_str()
	);
	std::shared_ptr<Mesh> cyllinder = std::make_shared<Mesh>("cyl",
		FixPath(L"../../meshes/cylinder.obj").c_str()
	);
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>("helix",
		FixPath(L"../../meshes/helix.obj").c_str()
	);
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>("quad",
		FixPath(L"../../meshes/quad.obj").c_str()
	);
	std::shared_ptr<Mesh> doubleSide = std::make_shared<Mesh>("doub",
		FixPath(L"../../meshes/quad_double_sided.obj").c_str()
	);
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>("torus",
		FixPath(L"../../meshes/torus.obj").c_str()
	);
	

	// mesh list
	meshes.insert(meshes.end(), { sph, cube, cyllinder, helix, quad, doubleSide, torus });

	// game entities
	// normies
	entities.push_back(
		std::make_shared<GameEntity>(
			sph,
			materials[0]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cube,
			materials[1]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cyllinder,
			materials[2]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			helix,
			materials[0]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			quad,
			materials[1]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			doubleSide,
			materials[2]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			torus,
			materials[0]
		));
	// 2
	entities.push_back(
		std::make_shared<GameEntity>(
			sph,
			materials[3]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cube,
			materials[3]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cyllinder,
			materials[3]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			helix,
			materials[3]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			quad,
			materials[3]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			doubleSide,
			materials[3]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			torus,
			materials[3]
		));
	//3
	entities.push_back(
		std::make_shared<GameEntity>(
			sph,
			materials[4]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cube,
			materials[4]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cyllinder,
			materials[4]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			helix,
			materials[4]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			quad,
			materials[4]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			doubleSide,
			materials[4]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			torus,
			materials[4]
		));
	// 4
	entities.push_back(
		std::make_shared<GameEntity>(
			sph,
			materials[5]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cube,
			materials[5]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			cyllinder,
			materials[5]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			helix,
			materials[5]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			quad,
			materials[5]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			doubleSide,
			materials[5]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			torus,
			materials[5]
		));


	// transforms
	entities[0]->GetTransform()->MoveAbsolute(-9, 0, 0);
	entities[1]->GetTransform()->MoveAbsolute(-6, 0, 0);
	entities[2]->GetTransform()->MoveAbsolute(-3, 0, 0);
	entities[3]->GetTransform()->MoveAbsolute(0, 0, 0);
	entities[4]->GetTransform()->MoveAbsolute(3, 0, 0);
	entities[5]->GetTransform()->MoveAbsolute(6, 0, 0);
	entities[6]->GetTransform()->MoveAbsolute(9, 0, 0);

	entities[7]->GetTransform()->MoveAbsolute(-9, 3, 0);
	entities[8]->GetTransform()->MoveAbsolute(-6, 3, 0);
	entities[9]->GetTransform()->MoveAbsolute(-3, 3, 0);
	entities[10]->GetTransform()->MoveAbsolute(0, 3, 0);
	entities[11]->GetTransform()->MoveAbsolute(3, 3, 0);
	entities[12]->GetTransform()->MoveAbsolute(6, 3, 0);
	entities[13]->GetTransform()->MoveAbsolute(9, 3, 0);

	entities[14]->GetTransform()->MoveAbsolute(-9, 6, 0);
	entities[15]->GetTransform()->MoveAbsolute(-6, 6, 0);
	entities[16]->GetTransform()->MoveAbsolute(-3, 6, 0);
	entities[17]->GetTransform()->MoveAbsolute(0, 6, 0);
	entities[18]->GetTransform()->MoveAbsolute(3, 6, 0);
	entities[19]->GetTransform()->MoveAbsolute(6, 6, 0);
	entities[20]->GetTransform()->MoveAbsolute(9, 6, 0);

	entities[21]->GetTransform()->MoveAbsolute(-9, -3, 0);
	entities[22]->GetTransform()->MoveAbsolute(-6, -3, 0);
	entities[23]->GetTransform()->MoveAbsolute(-3, -3, 0);
	entities[24]->GetTransform()->MoveAbsolute(0, -3, 0);
	entities[25]->GetTransform()->MoveAbsolute(3, -3, 0);
	entities[26]->GetTransform()->MoveAbsolute(6, -3, 0);
	entities[27]->GetTransform()->MoveAbsolute(9, -3, 0);


}

void Game::CreateLights()
{
	// directional lights
	Light light = {};
	light.type = LIGHT_TYPE_DIRECTIONAL;
	light.direction = DirectX::XMFLOAT3(1, -1, 0);
	light.color = DirectX::XMFLOAT3(0.2, 0.2, 1.0);
	light.intensity = 1.0f;

	Light dir2 = {};
	dir2.type = LIGHT_TYPE_DIRECTIONAL;
	dir2.intensity = 2.0f;
	dir2.color = DirectX::XMFLOAT3(1, 0, 0);
	dir2.direction = DirectX::XMFLOAT3(1, 0, 0);

	Light dir3 = {};
	dir3.type = LIGHT_TYPE_DIRECTIONAL;
	dir3.intensity = 1.5f;
	dir3.color = DirectX::XMFLOAT3(0, 0, 1);
	dir3.direction = DirectX::XMFLOAT3(0, 1, 0);

	// point lights
	Light point1 = {};
	point1.color = DirectX::XMFLOAT3(1, 1, 1);
	point1.type = LIGHT_TYPE_POINT;
	point1.intensity = 0.5;
	point1.position = DirectX::XMFLOAT3(0, 1.5f, 0);
	point1.range = 12.0f;

	Light point2 = {};
	point2.color = DirectX::XMFLOAT3(1, 1, 1);
	point2.type = LIGHT_TYPE_POINT;
	point2.intensity = 5.5;
	point2.position = DirectX::XMFLOAT3(-3.0f, 1.0f, 0);
	point2.range = 5.0f;

	// spot light
	Light spot = {};
	spot.color = XMFLOAT3(1, 1, 1);
	spot.type = LIGHT_TYPE_SPOT;
	spot.intensity = 2.0f;
	spot.position = XMFLOAT3(6.0f, 1.5f, 0);
	spot.direction = XMFLOAT3(0, -1, 0);
	spot.range = 10.0f;
	spot.spotOuterAngle = XMConvertToRadians(30.0f);
	spot.spotInnerAngle = XMConvertToRadians(20.0f);

	// add to lights
	lights.push_back(light);
	lights.push_back(dir2);
	lights.push_back(dir3);
	lights.push_back(point1);
	lights.push_back(point2);
	lights.push_back(spot);

	// taken from demo
	// Normalize directions of all non-point lights
	for (int i = 0; i < lights.size(); i++)
		if (lights[i].type != LIGHT_TYPE_POINT)
			XMStoreFloat3(
				&lights[i].direction,
				XMVector3Normalize(XMLoadFloat3(&lights[i].direction))
			);
}

// creates materials for drawing game objects with
void Game::CreateMaterials()
{
	//0
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		0.0f,
		vertexShader,
		pixelShader
	));
	//1
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		1.0f,
		vertexShader,
		pixelShader
	));
	//2
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		0.5f,
		vertexShader,
		pixelShader
	));
	//3
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		0.2f,
		vertexShader,
		uvPS
	));
	//4
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f),
		0.1f,
		vertexShader,
		normalsPS
	));
	//5
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		0.0f,
		vertexShader,
		pseudoPS
	));
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

	
	//ImGui::DragFloat3("Offset: ", &_offset.x, 0.01f);
	ImGui::ColorEdit4("Color Tint: ", &_colorTint.x);

	{
		int i = 0;
		for (auto& c : cameras) {

			ImGui::RadioButton(c->GetName(), &curCamera, i);
			i++;
		}

		ImGui::Text("Active Camera Information");
		ImGui::Text("Name: %s", cameras[curCamera]->GetName());
		//ImGui::Text("View Matrix"); ImGui::SameLine();
		ImGui::Text("Field of View: %f", cameras[curCamera]->GetFov());
		ImGui::Text("Near Clip: %f  |  Far Clip: %f", cameras[curCamera]->GetNearClip(), cameras[curCamera]->GetFarClip());
		ImGui::Text("Movement Speed: %f", cameras[curCamera]->GetMvmtSpd());
		ImGui::Text("Mouse Speed: %f", cameras[curCamera]->GetMouseSpd());
		DirectX::XMFLOAT4X4 viewMat = cameras[curCamera]->GetView();
		//ImGui::BeginTable("View: ", 4);
		for (int row = 0; row < 4; row++) {
			for (int column = 0; column < 4; column++) {
				ImGui::Text(" %f |", viewMat.m[row][column]);
				ImGui::SameLine();
			}
			ImGui::NewLine();
		}
		//ImGui::EndTable();
		

		ImGui::NewLine();

	}

	if (ImGui::TreeNode("Game Entity Information")) {
		for (auto& m : entities) {
			ImGui::Text("Name: %s", m->GetMesh()->GetName()); 
			ImGui::Text("Tris: %d | ", m->GetMesh()->GetIndexCount() / 3); ImGui::SameLine();
			ImGui::Text("Verts: %d | ", m->GetMesh()->GetVertexCount()); ImGui::SameLine();
			ImGui::Text("Indices: %d", m->GetMesh()->GetIndexCount());
			ImGui::Text("Position: (%f, %f, %f)", m->GetTransform()->GetPosition().x, m->GetTransform()->GetPosition().y, m->GetTransform()->GetPosition().z);
			ImGui::Text("Rotation: (%f, %f, %f)", m->GetTransform()->GetPitchYawRoll().x, m->GetTransform()->GetPitchYawRoll().y, m->GetTransform()->GetPitchYawRoll().z);
			ImGui::Text("Scale: (%f, %f, %f)", m->GetTransform()->GetScale().x, m->GetTransform()->GetScale().y, m->GetTransform()->GetScale().z);
			ImGui::NewLine();
		}
		ImGui::TreePop();
	}

	

	if (ImGui::TreeNode("Game Entity Controls")) {
		int i = 0;
		for (auto& e : entities) {
			//const char* name = "entity control for %d", i;
			//ImGui::Begin(name);
			ImGui::PushID(i);
			ImGui::Text("Name: %s %d", e->GetMesh()->GetName(), i);
			// position
			DirectX::XMFLOAT3 pos = e->GetTransform()->GetPosition();
			ImGui::DragFloat3("Position: ", &pos.x, 0.1f);
			e->GetTransform()->SetPosition(pos);
			// rotation
			DirectX::XMFLOAT3 rot = e->GetTransform()->GetPitchYawRoll();
			ImGui::DragFloat3("Rotation: ", &rot.x, 0.1f);
			e->GetTransform()->SetRotation(rot);
			// scale
			DirectX::XMFLOAT3 sca = e->GetTransform()->GetScale();
			ImGui::DragFloat3("Scale: ", &sca.x, 0.1f);
			e->GetTransform()->SetScale(sca);

			ImGui::NewLine();
			ImGui::PopID();
			//ImGui::End();
			i++;
		}
		ImGui::TreePop();
	}
	
	ImGui::End();
}

void Game::InitializeCamera()
{
	float aR = (float)Window::Width() / Window::Height();
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(aR, XMFLOAT3(0.5f, 0.5f, -5.0f), XMFLOAT3(0.0f, 0.5, 0.0f), DirectX::XM_PIDIV4, 0.01f, 100.0f, 3.0f, 0.002f, "camera 1");
	std::shared_ptr<Camera> cam2 = std::make_shared<Camera>(aR, XMFLOAT3(0.0f, 0.0f, -5.0f), "camera 2");

	cameras.push_back(camera);
	cameras.push_back(cam2);
}

void Game::UpdateObjectTransformations(float deltaTime)
{
	// alter second triangle rotation every frame
	DirectX::XMFLOAT3 triRot = entities[1]->GetTransform()->GetPitchYawRoll();
	triRot.z += 1.0f * deltaTime;
	entities[1]->GetTransform()->SetRotation(triRot);
	// alter second triangle position every frame
	DirectX::XMFLOAT3 triPos = entities[1]->GetTransform()->GetPosition();
	triPos.x += 0.000002f;
	entities[1]->GetTransform()->SetPosition(triPos);

	// alter second bunny scale every frame
	DirectX::XMFLOAT3 bunSca = entities[4]->GetTransform()->GetScale();
	bunSca.x += 0.00005f;
	bunSca.y += 0.00001f;
	entities[4]->GetTransform()->SetScale(bunSca);
	// alter second bunny position every frame
	DirectX::XMFLOAT3 bunPos = entities[4]->GetTransform()->GetPosition();
	bunPos.y -= 0.00005f;
	entities[4]->GetTransform()->SetPosition(bunPos);

}


// --------------------------------------------------------
// Handle resizing to match the new window size
// update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	float aR = (float)Window::Width() / Window::Height();	// get new aspect ratio
	// update projection matrix
	if (!cameras.empty()) {
		for (auto& c : cameras) {
			c->UpdateProjectionMatrix(aR);
		}
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	
	// update imgui info 
	GuiUpdate(deltaTime);
	BuildGui();

	//UpdateObjectTransformations(deltaTime);

	// update cameras
	for (auto& c : cameras) c->Update(deltaTime);
	
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
 
	}

	// DRAW geometry
	// - These steps are generally repeated for EACH object you draw
	// - Other Direct3D calls will also be necessary to do more complex things
	for (auto& g : entities) {
		g->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambient);
		g->GetMaterial()->GetPixelShader()->SetData(
			"lights", 
			&lights[0],
			sizeof(Light) * (int)lights.size()
		);
		g->Draw(_colorTint, cameras[curCamera]);
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



