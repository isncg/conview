#include "ConElement.h"
#include<Windows.h>
#include<vector>
#include<bitset>

#define DEFAULT_ATTR GetDefaultAttr()

unsigned long write_console_line(int x, int y, CHAR_INFO* buffer, int wchar_count) {
	SMALL_RECT srctWriteRect{ x, y, x + wchar_count * 2, y + 1 };
	COORD coordBufSize{ wchar_count, 1 };
	COORD coordBufCoord{ 0,0 };
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	GetLastError();
	auto r = WriteConsoleOutputW(
		hStdout, // screen buffer to write to 
		(CHAR_INFO*)buffer,      // buffer to copy from 
		coordBufSize,     // col-row size of chiBuffer 
		coordBufCoord,    // top left src cell in chiBuffer 
		&srctWriteRect);  // dest. screen buffer rectangle 
	auto err = GetLastError();
	return err;
}

int count_wchars_within_line(std::wstring & wstr, unsigned int offset, unsigned int size, int* actual_filled, unsigned int* end_index, bool break_after_return = true) {
	int filled = 0;
	int str_index = offset;
	while (filled < size && str_index < wstr.size()) {
		wchar_t ch = wstr[str_index];
		ch & 0xff00 ? filled += 2 : ++filled;
		++str_index;
		if (break_after_return && ch == '\n') {
			break;
		}
	}
	if (filled > size) {
		--str_index;
		filled -= 2;
	}
	*actual_filled = filled;
	*end_index = str_index;
	return str_index - offset;
}


class line_input_reader {
public:
	int read_index = 0;
	int line_size_in_byte = 0;
	std::wstring str;
	int return_mode = 0;
	enum RETURN_MODE {
		RETURN_MODE_IGNORE = 0,
		RETURN_MODE_SPACE = 1,
		RETURN_MODE_RETURN = 2,
		RETURN_MODE_SPACE_RETURN = 3,
	};
	void Begin(std::wstring & wstr, int line_size_in_byte, int return_mode = RETURN_MODE_RETURN) {
		str = wstr;
		read_index = 0;
		this->line_size_in_byte = line_size_in_byte;
		this->return_mode = return_mode;
	}

	void ReadToEnter(std::vector<wchar_t>& result) {
		int filled = 0;
		int return_char_count = 0;
		int read_index = this->read_index;
		result.clear();
		while (filled < line_size_in_byte && read_index < str.size()) {
			wchar_t ch = str[read_index];
			if (ch != '\n') {
				ch & 0xff00 ? filled += 2 : ++filled;
				read_index++;
			}
			else {
				break;
			}
		}
		if (filled > line_size_in_byte) {
			--read_index;
			filled -= 2;
		}

		for (int i = this->read_index; i < read_index; i++) {
			result.push_back(str[i]);
		}
		if (read_index < str.size() && str[read_index] == '\n') {
			++read_index;
		}
		this->read_index = read_index;
	}

	/*void Read(std::vector<wchar_t>& result) {
		int filled = 0;
		int return_char_count = 0;
		int read_index = this->read_index;
		result.clear();
		while (filled < line_size_in_byte && read_index < str.size()) {
			wchar_t ch = str[read_index];
			if (ch != '\n') {
				ch & 0xff00 ? filled += 2 : ++filled;
				read_index++;
			}
			else {
				++return_char_count;
				if (return_mode != RETURN_MODE_RETURN) {
					++filled;
				}
				if (return_mode & RETURN_MODE_SPACE) {
					++read_index;
				}
				if (return_mode | RETURN_MODE_RETURN) {
					break;
				}

			}
		}
		if (filled > line_size_in_byte) {
			--read_index;
			filled -= 2;
		}

		if ((return_mode & RETURN_MODE_SPACE) && return_char_count) {
			for (int i = this->read_index; i < read_index; i++) {
				if (str[i] == '\n') {
					result.push_back(' ');
				}
				else {
					result.push_back(str[i]);
				}
			}
		}
		else {
			for (int i = this->read_index; i < read_index; i++) {
				result.push_back(str[i]);
			}
		}
		this->read_index = read_index;
	}*/

};




int GetDefaultAttr() {
	/*static WORD attr = 0;
	if (attr) {
		return attr;
	}*/
	WORD attr = 0;
	COORD c{ 0,0 };
	DWORD r = 0;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	ReadConsoleOutputAttribute(hStdout, &attr, 1, c, &r);
	//printf("Console Attr: %d\n", attr);
	return attr;
}

void ConTextView::SetArea(int x, int y, int width, int height)
{
	ConView::SetArea(x, y, width, height);
	int attr = DEFAULT_ATTR;
	if (this->wtext_data.size()) {
		SetText(this->wtext_data);
	}
	SetStyle(attr);

}

std::wstring & ConTextView::GetText()
{
	return this->wtext_data;
	// TODO: 在此处插入 return 语句
}

//std::string & ConTextView::SetText(std::string & text)
//{
//	// TODO: 在此处插入 return 语句
//	this->text_data = text;
//	int max_size = width*height;
//	if (!max_size) {
//		return this->text_data;
//	}
//	int reader = 0;
//	int writer = 0;
//	auto char_info_buffer = (CHAR_INFO*)buffer;
//	while (reader < text_data.size() && writer < max_size)
//	{
//		auto ch = text_data[reader];
//		if ('\n' == ch) {
//
//		}
//		char_info_buffer[writer].Char.AsciiChar = ch;
//		reader++;
//		writer++;
//	}
//	//this->Update();
//	return this->text_data;
//}

std::wstring & ConTextView::SetText(std::wstring & text)
{
	// TODO: 在此处插入 return 语句
	this->wtext_data = text;
	int max_size = width * height;
	if (!max_size) {
		return this->wtext_data;
	}
	/*int reader = 0;
	int writer = 0;*/
	auto char_info_buffer = (CHAR_INFO*)buffer;
	//int char_read = 0;
	//int byte_filled = 0;
	/*while (char_read = count_wchars_within_line(text, reader, width, &byte_filled, true)) {

	}*/

	line_input_reader reader;
	reader.Begin(text, width);
	int row = 0;
	while (row < height) {
		std::vector<wchar_t> line;
		reader.ReadToEnter(line);
		if (line.size() <= 0) {
			break;
		}
		char* buf = new char[width] {0};
		int len = WideCharToMultiByte(CP_ACP, 0, &line[0], line.size(), buf, width, 0, 0);
		int col = 0;
		for (; col < len; col++) {
			char_info_buffer[row*width + col].Char.AsciiChar = buf[col];
		}
		for (; col < width; col++) {
			char_info_buffer[row*width + col].Char.AsciiChar = ' ';
		}
		delete buf;
		row++;
	}

	while (row < height) {		
		int col = 0;		
		for (; col < width; col++) {
			char_info_buffer[row*width + col].Char.AsciiChar = ' ';
		}		
		row++;
	}


	//while (reader < wtext_data.size() && writer < max_size)
	//{
	//	auto ch = wtext_data[reader];
	//	if ('\n' == ch) {

	//	}


	//	char_info_buffer[writer++].Char.UnicodeChar = ch;

	//	reader++;
	//	//writer++;
	//}

	//while (writer < max_size) {
	//	char_info_buffer[writer++].Char.UnicodeChar = ' ';
	//}

	//this->Update();
	return this->wtext_data;
}

void ConTextView::SetStyle(int style)
{
	int bufsize = width * height;
	auto char_info_buffer = (CHAR_INFO*)buffer;
	//int attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	for (int i = 0; i < bufsize; i++) {
		char_info_buffer[i].Attributes = style;
	}
}

void ConTextView::FitRows(int row_count)
{
	int col = wtext_data.size() / row_count;
	if (wtext_data.size() % row_count) {
		col++;
	}
	SetArea(x, y, col << 1, row_count);
}

ConTextView::ConTextView()
{

}

ConTextView::ConTextView(std::wstring & text)
{
	SetText(text);
	FitRows(1);
}

ConTextView::ConTextView(wchar_t * text)
{
	SetText(std::wstring(text));
	FitRows(1);
}


ConTextView::~ConTextView()
{
}




void ConView::Update()
{
	if (buffer) {
		SMALL_RECT srctWriteRect;
		COORD coordBufSize;
		COORD coordBufCoord;
		srctWriteRect.Top = y;    // top lt: row 10, col 0 
		srctWriteRect.Left = x;
		srctWriteRect.Bottom = y + height; // bot. rt: row 11, col 79 
		srctWriteRect.Right = x + width;


		// The temporary buffer size is 2 rows x 80 columns. 

		coordBufSize.Y = height;
		coordBufSize.X = width;

		// The top left destination cell of the temporary buffer is 
		// row 0, col 0. 

		coordBufCoord.X = 0;
		coordBufCoord.Y = 0;

		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hStdout == INVALID_HANDLE_VALUE)
		{
			printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
			return;
		}

		auto r = WriteConsoleOutputA(
			hStdout, // screen buffer to write to 
			(CHAR_INFO*)buffer,      // buffer to copy from 
			coordBufSize,     // col-row size of chiBuffer 
			coordBufCoord,    // top left src cell in chiBuffer 
			&srctWriteRect);  // dest. screen buffer rectangle 
		auto err = GetLastError();
	}
	for (auto i = Children.begin(); i != Children.end(); i++) {
		if (*i) (*i)->Update();
	}
}

void ConView::InitBuffer(int width, int height)
{
	this->width = width;
	this->height = height;
	int bufsize = this->width*this->height;

	if (this->buffer) {
		delete buffer;
		buffer = 0;
	}
	auto _buffer = new CHAR_INFO[bufsize];
	this->buffer = _buffer;
	//ZeroMemory(this->buffer, sizeof(CHAR_INFO)*bufsize);
	CHAR_INFO empty{ ' ', DEFAULT_ATTR };

	for (int i = 0; i < bufsize; i++) {
		_buffer[i] = empty;
	}



	/*HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hStdout == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
		return;
	}
	WORD attr = 0;
	COORD c{ 0,0 };
	DWORD r = 0;
	ReadConsoleOutputAttribute(hStdout, &attr, 1, c, &r);
	printf("Console Attr: %d\n", attr);*/
}

void ConView::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
}

void ConView::SetArea(int x, int y, int width, int height)
{
	InitBuffer(width, height);
	SetPosition(x, y);
}



ConView::ConView()
{
	SetArea(0, 0, 0, 0);
}

ConView::ConView(int x, int y, int width, int height)
{
	SetArea(x, y, width, height);
}



void ConTableElement::SetArea(int x, int y, int width, int height)
{
}

void ConTableElement::Update()
{
	//debug
	std::vector<std::vector<CHAR_INFO>> debug_buffer;
	for (int i = 0; i < height; i++) {
		std::vector<CHAR_INFO> cols(width);
		memcpy(&cols[0], (CHAR_INFO*)buffer + width * i, sizeof(CHAR_INFO)*width);
		debug_buffer.push_back(cols);
	}
	ConView::Update();
	int sum_top = 0;
	int sum_left = 0;

	sum_top = y;
	for (int i = 0; i < cells.size(); i++) {
		sum_left = x;
		for (int j = 0; j < cells[i].size(); j++) {
			if (cells[i][j]) {
				cells[i][j]->SetArea(sum_left + 1 + paddingH, sum_top + 1 + paddingV, col_width[j], row_height[i]);
				cells[i][j]->Update();
			}
			sum_left += col_width[j] + 1 + paddingH * 2;
		}
		sum_top += row_height[i] + 1 + paddingV * 2;
	}
}

void ConTableElement::FitCells()
{
	std::vector<int> row_height;
	std::vector<int> col_width;
	int max_col_count = 0;
	for (int i = 0; i < cells.size(); i++) {
		int max_height = 1;
		if (cells[i].size() > max_col_count) {
			max_col_count = cells[i].size();
		}
		for (int j = 0; j < cells[i].size(); j++) {
			if (cells[i][j] && cells[i][j]->height > max_height) {
				max_height = cells[i][j]->height;
			}
		}
		row_height.push_back(max_height);
	}
	for (int j = 0; j < max_col_count; j++) {
		int max_width = 2;
		for (int i = 0; i < cells.size(); i++) {
			if (j < cells[i].size() && cells[i][j] && cells[i][j]->width > max_width) {
				max_width = cells[i][j]->width;
			}
		}
		col_width.push_back(max_width);
	}

	SetLayout(row_height, col_width);
}

void ConTableElement::SetLayout(std::vector<int> row_height, std::vector<int> col_width)
{
	this->row_height = row_height;
	this->col_width = col_width;

	std::vector<std::vector<std::shared_ptr<ConView>>> new_cells;

	for (int i = 0; i < row_height.size(); i++) {
		std::vector<std::shared_ptr<ConView>> cell_row;
		if (i < cells.size()) {
			for (int j = 0; j < col_width.size(); j++) {
				if (j < cells[i].size()) {
					cell_row.push_back(cells[i][j]);
				}
				else {
					cell_row.push_back(std::shared_ptr<ConView>(NULL));
				}
			}
		}
		else {
			for (int j = 0; j < col_width.size(); j++) {
				cell_row.push_back(std::shared_ptr<ConView>(NULL));
			}
		}
		new_cells.push_back(cell_row);
	}
	cells = new_cells;


	//fill cell borders


	int total_width = 0;
	int total_height = 0;

	for (auto i = row_height.begin(); i != row_height.end(); i++) {
		total_height += (*i);
	}
	total_height += 2 + paddingV * 2 + (row_height.size() - 1) * (paddingV * 2 + 1);

	for (auto j = col_width.begin(); j != col_width.end(); j++) {
		total_width += (*j);
	}
	total_width += 2 + paddingH * 2 + (col_width.size() - 1) * (paddingH * 2 + 1);

	ConView::SetArea(x, y, total_width, total_height);
	auto char_info_buffer = (CHAR_INFO*)buffer;


	int sum_top = 0;
	int sum_left = 0;

	/*sum_top = 0;
	for (int i = 0; i < cells.size(); i++) {
	sum_left = 0;
	for (int j = 0; j < cells[i].size(); j++) {
	sum_left += col_width[j]+3;
	}
	sum_top += row_height[i]+3;
	}*/


	int attr = DEFAULT_ATTR;
	CHAR_INFO ch_X = { '+', attr };
	CHAR_INFO ch_H = { '-', attr };
	CHAR_INFO ch_V = { '|', attr };

	sum_top = 0;
	for (int i = 0; i < cells.size(); i++) {
		sum_left = 0;
		for (int j = 0; j < cells[i].size(); j++) {
			int bufindex = sum_top * total_width + sum_left;

			char_info_buffer[bufindex] = ch_X;
			for (int p = 0; p < col_width[j] + paddingH * 2; p++) {
				char_info_buffer[bufindex + p + 1] = ch_H;
			}

			for (int q = 0; q < row_height[i] + paddingV * 2; q++) {
				char_info_buffer[bufindex + (q + 1)*total_width] = ch_V;
			}

			sum_left += col_width[j] + 1 + paddingH * 2;
		}
		sum_top += row_height[i] + 1 + paddingV * 2;
	}

	sum_top = 0;
	for (int i = 0; i < cells.size(); i++) {
		int bufindex = sum_top * width + width - 1;
		char_info_buffer[bufindex] = ch_X;
		for (int q = 0; q < row_height[i] + paddingV * 2; q++) {
			char_info_buffer[bufindex + (q + 1)*total_width] = ch_V;
		}
		sum_top += row_height[i] + 1 + paddingV * 2;
	}
	sum_left = 0;
	for (int j = 0; j < col_width.size(); j++) {
		int bufindex = sum_top * total_width + sum_left;
		char_info_buffer[bufindex] = ch_X;
		for (int p = 0; p < col_width[j] + paddingH * 2; p++) {
			char_info_buffer[bufindex + p + 1] = ch_H;
		}
		sum_left += col_width[j] + 1 + paddingH * 2;
	}
	int bufindex = height * width - 1;
	char_info_buffer[bufindex] = ch_X;

	//debug
	std::vector<std::vector<CHAR_INFO>> debug_buffer;
	for (int i = 0; i < height; i++) {
		std::vector<CHAR_INFO> cols(width);
		memcpy(&cols[0], (CHAR_INFO*)buffer + width * i, sizeof(CHAR_INFO)*width);
		debug_buffer.push_back(cols);
	}

}

ConTableElement::ConTableElement(int rows, int columns)
{
	for (int i = 0; i < rows; i++) {
		std::vector<std::shared_ptr<ConView>> cell_row;
		for (int j = 0; j < columns; j++) {
			cell_row.push_back(std::shared_ptr<ConView>(NULL));
		}
		cells.push_back(cell_row);
	}
}

bool ConInput::IsFinishChar(wchar_t ch)
{
	if (ch == '\n' || ch == '\r') {
		return true;
	}
	return false;
}

void ConInput::OnStart()
{
}

void ConInput::OnFinish()
{
}

void ConInput::OnValueChanged()
{
}

void ConInput::OnPushed(std::vector<wchar_t>& wchars)
{
}

void ConInput::OnPoped(wchar_t ch)
{
}

void ConInput::OnControlKey(char key)
{
}

void ConInput::FinishInput()
{
	isFinish = true;
}



void ConInput::Input()
{
	isFinish = false;

	HANDLE hStdin;
	DWORD fdwSaveOldMode;
	DWORD cNumRead, fdwMode, i;
	INPUT_RECORD irInBuf[100];

	// Get the standard input handle. 

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if (hStdin == INVALID_HANDLE_VALUE) {
		puts("GetStdHandle");
		return;
	}

	// Save the current input mode, to be restored on exit. 

	if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
	{
		puts("GetConsoleMode");
		return;
	}

	// Enable the window and mouse input events. 

	fdwMode = ENABLE_WINDOW_INPUT;
	if (!SetConsoleMode(hStdin, fdwMode)) {
		puts("SetConsoleMode");
		exit(-1);
	}

	// Loop to read and handle the next 100 input events. 

	OnStart();

	while (!isFinish) {
		ZeroMemory(irInBuf, sizeof(irInBuf));
		if (!ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			sizeof(irInBuf) / sizeof(INPUT_RECORD),         // size of read buffer 
			&cNumRead)) // number of records read 
		{
			puts("ReadConsoleInput");
			break;
		}

		std::vector<char> input_bytes;
		std::vector<char> pop_buffer;
		wchar_t last_char;
		for (int i = 0; !isFinish && i < cNumRead; i++) {
			if (irInBuf[i].EventType == KEY_EVENT &&
				irInBuf[i].Event.KeyEvent.bKeyDown) {

				char ch = irInBuf[i].Event.KeyEvent.uChar.AsciiChar;

				switch (ch) {
				case 0:
					break;
				case '\b':
					if (!input.size())
						break;
					last_char = input.back();
					this->input.pop_back();
					this->OnValueChanged();
					this->OnPoped(last_char);
					break;
				default:

					if (IsFinishChar(ch)) {
						isFinish = true;
						break;
					}
					if ((ch & 0xff) < 0x1f || ch == 0x7f) {
						OnControlKey(ch);
					}
					else {
						input_bytes.push_back(ch);
					}

				}
			}

		}
		if (input_bytes.size()) {

			int bufsize = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, &input_bytes[0], input_bytes.size(), 0, 0);
			std::vector<wchar_t> buf(bufsize);
			MultiByteToWideChar(CP_ACP, MB_COMPOSITE, &input_bytes[0], input_bytes.size(), &buf[0], bufsize);

			for (auto ch : buf) {
				input.push_back(ch);
			}
			this->OnValueChanged();
			this->OnPushed(buf);
		}
	}

	SetConsoleMode(hStdin, fdwSaveOldMode);

}

void ConInputTest::OnValueChanged()
{

	auto s = input;
	s.push_back(0);
	printf("[%d][%ls]\n", input.size(), &s[0]);
	//std::wcout << &s[0] << std::endl;
}

void ConInputTest::OnPushed(std::vector<wchar_t>& wchars)
{
	auto s = wchars;
	s.push_back(0);
	printf(" + [%ls]\n", &s[0]);
}

void ConInputTest::OnPoped(wchar_t ch)
{
	printf(" + [%lc]\n", ch);
}

void ConInputText::OnValueChanged()
{
	auto s = input;
	s.push_back(0);
	std::wstring ss(&s[0]);
	this->SetText(label+ L": " + ss);
	this->Update();
}

ConInputText::ConInputText()
{
	auto s = input;
	s.push_back(0);
	std::wstring ss(&s[0]);
	this->SetText(label + L": " + ss);
}

ConInputText::ConInputText(std::wstring & lable)
{
	this->label = label;
	ConInputText();
}

ConInputText::ConInputText(wchar_t * lable)
{
	this->label = std::wstring(label);
	ConInputText();
}
