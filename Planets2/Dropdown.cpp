#include "Dropdown.h"

int Dropdown::get_render_height() const
{
	if (active) {
		return (choices.size() + 1) * rect.height;
	}
	else {
		return rect.height;
	}
}

bool Dropdown::contains_point(Vector2 point) const {
	int height = get_render_height();

	return point.x >= rect.x and point.x < rect.x + rect.width
		and point.y >= rect.y and point.y < rect.y + height;

}

int Dropdown::translate_click() const
{
	int diff_y = GetMouseY() - rect.y;

	int click_selection = diff_y / rect.height;

	if (click_selection == 0) { // Clicked the base box.
		return selected; // return previous selection.
	}

	return click_selection - 1;
}



std::string Dropdown::get_selected() const
{
	if (has_selected()) {
		return choices[selected];
	}
	else {
		return "";
	}
}

void Dropdown::click()
{
	if (active) {
		selected = translate_click();
		callback(get_selected());
		deactivate();
	}
}

void Dropdown::render() const
{


	Rectangle render_rect = rect;

	// set render rectangle height to accomodate all the choices, if active.
	render_rect.height = get_render_height();

	// draw background
	DrawRectangleRec(render_rect, background_color);
	DrawRectangleLinesEx(render_rect, edge_width, edge_color);


	int start_x = rect.x + edge_width + rect.width * (width_padding / 2);
	int top_y = rect.y + edge_width; // y coord of the bottom of the top edge.
	int full_width = rect.height - 2 * edge_width; // distance from bottom of top edge to top of bottom edge.
	int start_y = top_y + full_width / 3;

	if (active) { // render choices list
		constexpr float SEPARATOR_THICKNESS = 3.0f;

		// draw separator lines
		Vector2 sep_left = { render_rect.x + edge_width, 0 };
		Vector2 sep_right = { render_rect.x - edge_width + render_rect.width, 0 };

		for (int i = 1; i < choices.size() + 1; i++) {
			sep_left.y = render_rect.y + rect.height * i;
			sep_right.y = sep_left.y;
			DrawLineEx(sep_left, sep_right, SEPARATOR_THICKNESS, GRAY);
		}

		// draw choices.
		for (int i = 0; i < choices.size(); i++) {
			const std::string& choice = choices[i];

			DrawText(choice.c_str(), start_x, start_y + rect.height * (i+1), font_size, LIGHTGRAY);
		}
	}
	

	if (has_selected()) { // if selected an option, draw current choice in the base box.
		DrawText(get_selected().c_str(), start_x, start_y, font_size, WHITE);
	}


	
}

void Dropdown::set_on_selection(std::function<void(const std::string& text)> to_set)
{
	callback = to_set;
}
