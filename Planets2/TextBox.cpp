#include "TextBox.h"

void TextBox::click() {
	double mouse_x_in_box = GetMouseX() - get_start_x_text();

	double mouse_pct = mouse_x_in_box / MeasureText(entered_text.c_str(), font_size);

	int len_text = entered_text.size();
	cursor_pos = std::min(len_text, static_cast<int>(mouse_pct * len_text));

}

void TextBox::render() const {
	DrawRectangleRec(rect, background_color);
	DrawRectangleLinesEx(rect, edge_width, edge_color);

	int start_x = get_start_x_text();
	int top_y = rect.y + edge_width;
	int full_width = rect.height - 2 * edge_width;
	int start_y = top_y + full_width / 3;

	if (entered_text.empty()) {
		DrawText(prompt_text.c_str(), start_x, start_y, font_size, LIGHTGRAY);
	}
	else {
		DrawText(entered_text.c_str(), start_x, start_y, font_size, WHITE);
	}

	
	if (active) {
		constexpr int CURSOR_LINE_PADDING = 3;
		constexpr double CURSOR_LINE_PADDING_HEIGHT = 0.2;
		int cursor_line_x = start_x + MeasureText(entered_text.substr(0, cursor_pos).c_str(), font_size) + CURSOR_LINE_PADDING;
		int cursor_line_y = top_y + full_width * CURSOR_LINE_PADDING_HEIGHT;
		int cursor_line_y_end = top_y + full_width * (1 - CURSOR_LINE_PADDING_HEIGHT);

		DrawLine(cursor_line_x, cursor_line_y, cursor_line_x, cursor_line_y_end, WHITE);
	}
}

bool TextBox::send_keypress(int key_code)
{
	if (key_code == KEY_BACKSPACE) {
		if (cursor_pos > 0) {
			cursor_pos--;
			entered_text.erase(cursor_pos, 1);
		}

		return true;
	}

	// if key is invalid.
	if (key_code < 32 or key_code > 126) {
		return false;
	}

	entered_text.insert(cursor_pos, 1, static_cast<char>(key_code));
	cursor_pos++;

	return true;
}
