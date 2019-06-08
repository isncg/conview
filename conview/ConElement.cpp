#include "ConElement.h"
#include<Windows.h>
#include<vector>
#include<bitset>

#define DEFAULT_ATTR GetDefaultAttr()

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
	SetText(this->text_data);
	SetStyle(attr);

}

std::string & ConTextView::GetText()
{
	return this->text_data;
	// TODO: �ڴ˴����� return ���
}

std::string & ConTextView::SetText(std::string & text)
{
	// TODO: �ڴ˴����� return ���
	this->text_data = text;
	int max_size = width*height;
	if (!max_size) {
		return this->text_data;
	}
	int reader = 0;
	int writer = 0;
	auto char_info_buffer = (CHAR_INFO*)buffer;
	while (reader < text_data.size() && writer < max_size)
	{
		auto ch = text_data[reader];
		if ('\n' == ch) {

		}
		char_info_buffer[writer].Char.AsciiChar = ch;
		reader++;
		writer++;
	}
	//this->Update();
	return this->text_data;
}

void ConTextView::SetStyle(int style)
{
	int bufsize = width*height;
	auto char_info_buffer = (CHAR_INFO*)buffer;
	//int attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	for (int i = 0; i < bufsize; i++) {
		char_info_buffer[i].Attributes = style;
	}
}

void ConTextView::FitRows(int row_count)
{
	int col = text_data.size() / row_count;
	if (text_data.size() % row_count) {
		col++;
	}
	SetArea(x, y, col, row_count);
}

ConTextView::ConTextView()
{

}

ConTextView::ConTextView(std::string & text)
{
	SetText(text);
	FitRows(1);
}

ConTextView::ConTextView(char * text)
{
	SetText(std::string(text));
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

		auto r = WriteConsoleOutput(
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
	CHAR_INFO empty{ 0, DEFAULT_ATTR };

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
		memcpy(&cols[0], (CHAR_INFO*)buffer + width*i, sizeof(CHAR_INFO)*width);
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
			int bufindex = sum_top*total_width + sum_left;

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
		int bufindex = sum_top*width + width - 1;
		char_info_buffer[bufindex] = ch_X;
		for (int q = 0; q < row_height[i] + paddingV * 2; q++) {
			char_info_buffer[bufindex + (q + 1)*total_width] = ch_V;
		}
		sum_top += row_height[i] + 1 + paddingV * 2;
	}
	sum_left = 0;
	for (int j = 0; j < col_width.size(); j++) {
		int bufindex = sum_top*total_width + sum_left;
		char_info_buffer[bufindex] = ch_X;
		for (int p = 0; p < col_width[j] + paddingH * 2; p++) {
			char_info_buffer[bufindex + p + 1] = ch_H;
		}
		sum_left += col_width[j] + 1 + paddingH * 2;
	}
	int bufindex = height*width - 1;
	char_info_buffer[bufindex] = ch_X;

	//debug
	std::vector<std::vector<CHAR_INFO>> debug_buffer;
	for (int i = 0; i < height; i++) {
		std::vector<CHAR_INFO> cols(width);
		memcpy(&cols[0], (CHAR_INFO*)buffer + width*i, sizeof(CHAR_INFO)*width);
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
}

void ConInputTest::OnPoped(wchar_t ch)
{
}

