#include <raylib.h>
#include <iostream>
#include <format>
#include <string>

#include "universe.h"

//#define MY_DEBUG
#define RENDER_QUAD_TREE

float screen_width = 1700.0;
float screen_height = 900.0;

bool running;

void zoom_in(Camera2D& camera) {
	camera.zoom *= 2;
}

void zoom_out(Camera2D& camera) {
	camera.zoom /= 2;
}

void process_input(Universe& universe, Camera2D& camera) {
	static int multiplier = 8;
	static float cam_speed = 5 * multiplier / camera.zoom;


	if (IsKeyDown(KEY_W)) {
		camera.target.y -= cam_speed;
	}
	if (IsKeyDown(KEY_A)) {
		camera.target.x -= cam_speed;
	}
	if (IsKeyDown(KEY_S)) {
		camera.target.y += cam_speed;
	}
	if (IsKeyDown(KEY_D)) {
		camera.target.x += cam_speed;
	}

	if (IsKeyPressed(KEY_SPACE)) {
		running = !running;
	}

	if (IsKeyPressed(KEY_COMMA)) {
		zoom_out(camera);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_PERIOD)) {
		zoom_in(camera);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_MINUS)) {
		multiplier = std::max(1, multiplier - 1);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (IsKeyPressed(KEY_EQUAL)) {
		++multiplier;
		cam_speed = 5 * multiplier / camera.zoom;
	}

	float wheel_move = GetMouseWheelMove();

	if (wheel_move < 0) {
		zoom_out(camera);
		cam_speed = 5 * multiplier / camera.zoom;
	}
	else if (wheel_move > 0) {
		zoom_in(camera);
		cam_speed = 5 * multiplier / camera.zoom;
	}

}

bool on_screen(const Body& body, Camera2D& camera) {
	Vector2 leftmost = GetWorldToScreen2D({ body.x - body.radius, body.y }, camera);
	Vector2 rightmost = GetWorldToScreen2D({ body.x + body.radius, body.y }, camera);

	Vector2 lowest = GetWorldToScreen2D({ body.x, body.y + body.radius }, camera);
	Vector2 highest = GetWorldToScreen2D({ body.x, body.y - body.radius }, camera);

	// can optimize : screen_pos.x >= -body.radius && screen_pos.y >= -body.radius

	return rightmost.x >= 0 and lowest.y >= 0 and leftmost.x < GetScreenWidth() and highest.y < GetScreenHeight();
}

#ifdef RENDER_QUAD_TREE
void draw_quad(const QuadTree& cur) {
	Rectangle rec{ cur.x, cur.y, cur.get_width(), cur.get_height() };

	DrawRectangleLinesEx(rec, 50, RAYWHITE);


	if (!cur.is_leaf()) {
		const std::array<QuadTree*, 4> quads = cur.get_quads();

		for (int i = 0; i < 4; i++) {
			draw_quad(*quads[i]);
		}
	}
}
#endif

void render_system(Universe& universe, Camera2D& camera) {
	const std::vector<std::unique_ptr<Body>>& bodies = universe.get_bodies();

#ifdef RENDER_QUAD_TREE

	const QuadTree& cur = universe.get_quad_root();

	draw_quad(cur);

#endif

	for (const auto &body_ptr : bodies) {
		const Body& body = *body_ptr;

		if (on_screen(body, camera)) {
			Color planet_color = body.type->color; // not a reference
			DrawCircle(body.x, body.y, body.radius, planet_color);

#ifdef MY_DEBUG
			int text_x = body.x + body.radius + 20;
			int text_y = body.y + body.radius + 20;
			int font_size = 25;
			int spacing = 20;
			std::string id_str = std::format("ID: {:}", body.id);
			std::string x_str = std::format("X: {:.2f}", body.x);
			std::string y_str = std::format("Y: {:.2f}", body.y);
			std::string vel_x_str = std::format("Vel(x): {:.2f}", body.vel_x);
			std::string vel_y_str = std::format("Vel(y): {:.2f}", body.vel_y);
			std::string mass_str = std::format("Mass: {:}", body.mass);

			DrawText(id_str.c_str(), text_x, text_y, font_size, planet_color);
			DrawText(x_str.c_str(), text_x, text_y + spacing, font_size, planet_color);
			DrawText(y_str.c_str(), text_x, text_y + spacing * 2, font_size, planet_color);
			DrawText(vel_x_str.c_str(), text_x, text_y + spacing * 3, font_size, planet_color);
			DrawText(vel_y_str.c_str(), text_x, text_y + spacing * 4, font_size, planet_color);
			DrawText(mass_str.c_str(), text_x, text_y + spacing * 5, font_size, planet_color);
#endif
		}


	}
}

int main() {
	std::ios_base::sync_with_stdio(false);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(screen_width, screen_height, "Game");
	SetExitKey(KEY_NULL);

	Universe universe;

	Camera2D camera;
	camera.offset = { static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2 };
	camera.target = { 0, 0 };
	camera.rotation = 0;
	camera.zoom = 1.0f;

	int rand_planets = 0;
	int systems = 1;

	for (int i = 0; i < rand_planets; ++i) {
		universe.create_rand_body();
	}

	for (int i = 0; i < systems; ++i) {
		universe.create_rand_system();
	}

	running = false;

	while (!WindowShouldClose()) {

		if (IsWindowResized()) {
			camera.offset = { static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2 };
		}

		process_input(universe, camera);

		if (running) {
			universe.update();
		}

		BeginDrawing();
			ClearBackground(BLACK); // maybe better to have before beginmode2d?

			

			BeginMode2D(camera);
				render_system(universe, camera);
			EndMode2D();

			DrawFPS(50, 50);

			std::string num_bodies_str = "Number bodies: " + std::to_string(universe.get_num_bodies());

			DrawText(num_bodies_str.c_str(), 50, 70, 20, RAYWHITE);

		EndDrawing();
	}


	CloseWindow();

	return 0;
}