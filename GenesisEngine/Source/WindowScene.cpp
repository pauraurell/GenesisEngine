#include "WindowScene.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "glew/include/glew.h"
#include "WindowAssets.h"
#include "Time.h"


WindowScene::WindowScene() : EditorWindow()
{
	type = WindowType::SCENE_WINDOW;
}

WindowScene::~WindowScene()
{
}

void WindowScene::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Scene", &visible, ImGuiWindowFlags_MenuBar))
	{
		App->editor->scene_window_focused = ImGui::IsWindowFocused();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Display"))
			{
				if (ImGui::BeginMenu("Shading Mode"))
				{
					if (ImGui::MenuItem("Solid", NULL, App->renderer3D->display_mode == DisplayMode::SOLID))
						App->renderer3D->SetDisplayMode(DisplayMode::SOLID);
					if (ImGui::MenuItem("Wireframe", NULL, App->renderer3D->display_mode == DisplayMode::WIREFRAME))
						App->renderer3D->SetDisplayMode(DisplayMode::WIREFRAME);
					ImGui::EndMenu();
				}

				static bool vertex_normals = App->renderer3D->draw_vertex_normals;
				if (ImGui::Checkbox("Vertex Normals", &vertex_normals))
					App->renderer3D->draw_vertex_normals = vertex_normals;

				static bool face_normals = App->renderer3D->draw_face_normals;
				if (ImGui::Checkbox("Face Normals", &face_normals))
					App->renderer3D->draw_face_normals = face_normals;

				ImGui::EndMenu();
			}

			static bool lighting = glIsEnabled(GL_LIGHTING);
			if (ImGui::Checkbox("Lighting", &lighting))
				App->renderer3D->SetCapActive(GL_LIGHTING, lighting);

			static bool show_grid = App->scene->show_grid;
			if (ImGui::Checkbox("Show Grid", &show_grid))
				App->scene->show_grid = show_grid;

			ImGui::Checkbox("Cull editor camera", &App->renderer3D->cullEditorCamera);

			ImGui::EndMenuBar();
		}

		ImVec2 window_size = ImGui::GetContentRegionAvail();
		App->editor->sceneWindowOrigin = ImGui::GetWindowPos();
		App->editor->sceneWindowOrigin.x += ImGui::GetWindowContentRegionMin().x;
		App->editor->sceneWindowOrigin.y += ImGui::GetWindowContentRegionMin().y;

		App->editor->mouseScenePosition.x = App->input->GetMouseX() - App->editor->sceneWindowOrigin.x;
		App->editor->mouseScenePosition.y = App->input->GetMouseY() - App->editor->sceneWindowOrigin.y;

		if (App->in_game)
			DrawGameTimeDataOverlay();

		if (App->editor->image_size.x != window_size.x || App->editor->image_size.y != window_size.y)
			App->editor->OnResize(window_size);

		ImGui::Image((ImTextureID)App->renderer3D->colorTexture, App->editor->image_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::PushID(SCENE_WINDOW);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				WindowAssets* assets_window = (WindowAssets*)App->editor->windows[ASSETS_WINDOW];
				const char* file = assets_window->GetFileAt(payload_n);
				App->scene->AddGameObject(App->resources->RequestGameObject(file));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();

		App->scene->EditTransform();
		//ImGui::Image((ImTextureID)App->renderer3D->depthTexture, image_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void WindowScene::DrawGameTimeDataOverlay()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	window_flags |= ImGuiWindowFlags_NoMove;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 window_pos = App->editor->sceneWindowOrigin;
	window_pos.x += 10.0f;
	window_pos.y += 10.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
	ImGui::SetNextWindowPos(window_pos);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.25f, 0.25f, 0.25f, 0.75f));
	bool dummy_bool = true;
	if (ImGui::Begin("Example: Simple overlay", &dummy_bool, window_flags))
	{
		ImGui::Text("Game Time");
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Delta time %.3f", Time::gameClock.dt);
		ImGui::Text("Time Scale: %.2f", Time::gameClock.timeScale);
		ImGui::Text("Time since game start: %.2f", Time::gameClock.timeSinceStartup());
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::End();
}