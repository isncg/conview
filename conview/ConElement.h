#pragma once
#include<string>
#include<list>
#include<vector>
#include<memory>
#include<iostream>
#include<sstream>

#define New(cls, ...) std::shared_ptr<cls>(new cls(__VA_ARGS__))


class ConView {
public:
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	void* buffer = 0;
	virtual void Update();
	virtual void InitBuffer(int width, int height);
	virtual void SetPosition(int x, int y);
	virtual void SetArea(int x, int y, int width, int height);

	ConView();
	ConView(int x, int y, int width, int height);

	std::list<ConView*> Children;
	ConView* Parent;
};

class ConTextView : public ConView
{
	//std::string text_data;
	std::wstring wtext_data;
public:

	virtual void SetArea(int x, int y, int width, int height) override;
	std::wstring & GetText();
	//std::string & SetText(std::string& text);
	std::wstring & SetText(std::wstring& text);
	void SetStyle(int style);
	void FitRows(int row_count);
	void FitColumns(int col_count);

	ConTextView();
	ConTextView(std::wstring& text);
	ConTextView(wchar_t* text);
	~ConTextView();
};


class ConBorderElement {

};


class ConTableElement : public ConView {
	std::vector<int> row_height;
	std::vector<int> col_width;

	virtual void SetArea(int x, int y, int width, int height) override;
public:
	int paddingH = 1;
	int paddingV = 0;
	virtual void Update() override;
	void FitCells();
	std::vector<std::vector<std::shared_ptr<ConView>>> cells;
	void SetLayout(std::vector<int> row_height, std::vector<int> col_width);

	ConTableElement(int rows, int columns);
};


class ConInput {
	bool isFinish = false;

protected:
	std::vector<wchar_t> input;
	virtual bool IsFinishChar(wchar_t ch);
	virtual void OnStart();
	virtual void OnFinish();
	virtual void OnValueChanged();
	virtual void OnPushed(std::vector<wchar_t>& wchars);
	virtual void OnPoped(wchar_t ch);
	virtual void OnControlKey(char key);
	void FinishInput();
public:

	void Input();
	//void UnBind();
};


class ConInputText: public ConInput, public ConTextView {
	std::wstring label;
	virtual void OnValueChanged() override;
public:
	ConInputText();
	ConInputText(std::wstring & lable);
	ConInputText(wchar_t* lable);
};


class ConInputTest : public ConInput {
	virtual void OnValueChanged() override;
	virtual void OnPushed(std::vector<wchar_t>& wchars)override;
	virtual void OnPoped(wchar_t ch)override;
};