

#include "ConElement.h"
#include<locale>

int main() {
	setlocale(LC_ALL, "");

	//text view

	//ConTextView e1;
	//e1.SetArea(1, 1, 8, 8);
	//e1.SetText(std::wstring(L"一二\n一二三\n一二三四\n一二三s四\n一二三四wu\n"));
	//e1.Update();

	//table view

	//ConTableElement t1(5,5);
	//t1.SetPosition(20, 4);
	////t1.SetLayout({ 2, 1, 1 }, { 10, 10 });
	//t1.cells[0][0] = New(ConTextView, "Hello!"); 
	//t1.cells[1][0] = New(ConTextView, "Hello, world!");
	//t1.cells[2][0] = New(ConTextView, "Helloworld");
	//t1.FitCells();
	//t1.Update();

	//input base
	/*ConInputTest it;
	it.Input();*/

	//input text view

	ConInputText it;
	it.SetArea(1, 1, 10, 4);
	it.Update();
	it.Input();
	system("pause");
}