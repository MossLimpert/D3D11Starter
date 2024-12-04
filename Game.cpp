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
#include "BufferStructs.h"		// Assignment 4
#include "Material.h"			// Assignment 7
#include "WICTextureLoader.h"

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
XMFLOAT2 _uvScale(0.0f, 0.0f);
XMFLOAT2 _uvOffset(0.0f, 0.0f);

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
	ambient = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
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
			materials[1]
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
			materials[0]
		));
	entities.push_back(
		std::make_shared<GameEntity>(
			torus,
			materials[0]
		));


	// transforms
	entities[0]->GetTransform()->MoveAbsolute(-9, 0, 0);
	entities[1]->GetTransform()->MoveAbsolute(-6, 0, 0);
	entities[2]->GetTransform()->MoveAbsolute(-3, 0, 0);
	entities[3]->GetTransform()->MoveAbsolute(0, 0, 0);
	entities[4]->GetTransform()->MoveAbsolute(3, 0, 0);
	entities[5]->GetTransform()->MoveAbsolute(6, 0, 0);
	entities[6]->GetTransform()->MoveAbsolute(9, 0, 0);

	/*entities[7]->GetTransform()->MoveAbsolute(-9, 3, 0);
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
	entities[27]->GetTransform()->MoveAbsolute(9, -3, 0);*/


}

void Game::CreateLights()
{
	// directional lights
	Light light = {};
	light.type = LIGHT_TYPE_DIRECTIONAL;
	light.direction = DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f);
	light.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	light.intensity = 1.0f;

	Light dir2 = {};
	dir2.type = LIGHT_TYPE_DIRECTIONAL;
	dir2.intensity = 1.0f;
	dir2.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	dir2.direction = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);

	Light dir3 = {};
	dir3.type = LIGHT_TYPE_DIRECTIONAL;
	dir3.intensity = 1.0f;
	dir3.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	dir3.direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

	// point lights
	Light point1 = {};
	point1.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	point1.type = LIGHT_TYPE_POINT;
	point1.intensity = 0.5f;
	point1.position = DirectX::XMFLOAT3(0.0f, 1.5f, 0.0f);
	point1.range = 1.0f;

	Light point2 = {};
	point2.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	point2.type = LIGHT_TYPE_POINT;
	point2.intensity = 0.5f;
	point2.position = DirectX::XMFLOAT3(-3.0f, 1.0f, 0.0f);
	point2.range = 1.0f;

	// spot light
	Light spot = {};
	spot.color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	spot.type = LIGHT_TYPE_SPOT;
	spot.intensity = 1.0f;
	spot.position = XMFLOAT3(6.0f, 1.5f, 0.0f);
	spot.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	spot.range = 5.0f;
	spot.spotOuterAngle = XMConvertToRadians(20.0f);
	spot.spotInnerAngle = XMConvertToRadians(10.0f);

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
	// create sampler
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// outside U,V 0-1 then what
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	// wrap it!
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_ANISOTROPIC;		// what do do if we sample between pixels
	desc.MaxAnisotropy = 16;					// allows textures to look good at bad angles
	desc.MaxLOD = D3D11_FLOAT32_MAX;	// mip mapping at any range
	HRESULT stateCheck = Graphics::Device->CreateSamplerState(&desc, sampler.GetAddressOf());

	// declare textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> test;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tx2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> spec;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> spec2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tx_n_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tx_n_2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nm_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nm_2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> flatNormal;

	// fill textures
	HRESULT res = CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Specular Maps/brokentiles.png").c_str(),
		0,
		&test);
	
	HRESULT res2 = CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Specular Maps/rustymetal.png").c_str(),
		0,
		&tx2
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Specular Maps/brokentiles_specular.png").c_str(),
		0,
		&spec
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Specular Maps/rustymetal_specular.png").c_str(),
		0,
		&spec2
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Normal Maps/cushion.png").c_str(),
		0,
		&tx_n_1
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Normal Maps/rock.png").c_str(),
		0,
		&tx_n_2
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Normal Maps/cushion_normals.png").c_str(),
		0,
		&nm_1
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Normal Maps/rock_normals.png").c_str(),
		0,
		&nm_2
	);
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../textures/Normal Maps/flat_normals.png").c_str(),
		0,
		&flatNormal
	);

	// make materials
	//0
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		0.0f,
		vertexShader,
		pixelShader,
		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		0
	));
	materials[0]->AddTextureSRV("SurfaceTexture", tx_n_1);
	materials[0]->AddSampler("BasicSampler", sampler);
	materials[0]->AddTextureSRV("NormalMap", nm_1);
	//materials[0]->AddTextureSRV("SpecularMap", spec);
	
	//1
	materials.push_back(std::make_shared<Material>(
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		1.0f,
		vertexShader,
		pixelShader,
		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
		0
	));
	materials[1]->AddTextureSRV("SurfaceTexture", tx_n_2);
	materials[1]->AddSampler("BasicSampler", sampler);
	materials[1]->AddTextureSRV("NormalMap", nm_2);
	//materials[1]->AddTextureSRV("SpecularMap", spec2);

	std::shared_ptr<SimpleVertexShader> skyVS = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"SkyVS.cso").c_str());
	std::shared_ptr<SimplePixelShader> skyPS = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"SkyPS.cso").c_str());

	sky = std::make_shared<Sky>(
		FixPath(L"../../../textures/Skies/right.png").c_str(),
		FixPath(L"../../../textures/Skies/right.png").c_str(),
		FixPath(L"../../../textures/Skies/right.png").c_str(),
		FixPath(L"../../../textures/Skies/right.png").c_str(),
		FixPath(L"../../../textures/Skies/right.png").c_str(),
		FixPath(L"../../../textures/Skies/right.png").c_str(),
		entities[1]->GetMesh(),
		skyVS,
		skyPS,
		sampler
	);
	
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
	ImGui::ColorEdit4("ambient: ", &ambient.x);

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

	if (ImGui::TreeNode("Light Controls")) {
		int i = 100;	// starting it higher jic it having the same id as above
						// would mess with it
		for (auto& l : lights) {
			ImGui::PushID(i);

			// display type
			{
				const char* type = "";
				switch (l.type) {
				case 0:
					type = "Directional";
					break;
				case 1:
					type = "Point";
					break;
				case 2:
					type = "Spot";
					break;
				}
				ImGui::Text("Type: %s", type);
			}
			// change position if not directional else show position
			{
				DirectX::XMFLOAT3 pos = l.position;
				if (l.type != 0) {
					ImGui::DragFloat3("Position", &pos.x, 0.1f);
					l.position = pos;
				}
				else {
					ImGui::Text("Position: x: %f | y: %f | z: %f", pos.x, pos.y, pos.z);
				}
			}
			// change direction if not point
			{
				DirectX::XMFLOAT3 dir = l.direction;
				if (l.type != 1) {
					ImGui::DragFloat3("Direction", &dir.x, 0.1f);
					l.direction = dir;
				}
				else {
					ImGui::Text("Direction: x: %f | y: %f | z: %f");
				}
			}
			// change color
			DirectX::XMFLOAT3 col = l.color;
			ImGui::ColorEdit3("Color", &col.x);
			l.color = col;
			// change range if not directional
			if (l.type != 0) {
				float r = l.range;
				ImGui::DragFloat("Range", &r, 0.1f);
				l.range = r;
			}
			// change intensity
			float k = l.intensity;
			ImGui::DragFloat("Intensity", &k, 0.1f);
			l.intensity = k;
			// if spot: change inner outer
			if (l.type == 2) {
				float m = l.spotInnerAngle;
				float n = l.spotOuterAngle;
				ImGui::DragFloat("Spot Inner Angle", &m, 0.1f);
				ImGui::DragFloat("Spot Outer Angle", &n, 0.1f);
				l.spotInnerAngle = m;
				l.spotOuterAngle = n;
			}

			ImGui::PopID();
			i++;
		}
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Materials")) {
		int i = 1000;

		for (auto& m : materials) {
			ImGui::PushID(i);

			DirectX::XMFLOAT2 offset = m->GetUVOffset();
			DirectX::XMFLOAT2 scale = m->GetUVScale();
			ImGui::DragFloat2("UV Offset", &offset.x, 0.1f);
			ImGui::DragFloat2("UV Scale", &scale.x, 0.1f);
			m->SetUVOffset(offset);
			m->SetUVScale(scale);

			ImGui::PopID();
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
		g->GetMaterial()->GetPixelShader()->SetData(
			"lights", 
			&lights[0],
			sizeof(Light) * (int)lights.size()
		);
		g->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambient);
		g->Draw(_colorTint, cameras[curCamera]);
	}
	
	// sky
	sky->Draw(cameras[curCamera]);

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



