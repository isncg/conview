

#include "ConElement.h"
#include<locale>

int main() {
	setlocale(LC_ALL, "");
	ConTextView e1;
	
	e1.SetArea(1, 1, 8, 4);
	e1.SetText(std::wstring(L"你好你好你好你好h你好h你好h你好h你好h你好h你好h你好"));
	e1.Update();
	//ConTableElement t1(5,5);
	//t1.SetPosition(20, 4);
	////t1.SetLayout({ 2, 1, 1 }, { 10, 10 });
	//t1.cells[0][0] = New(ConTextView, "Hello!"); 
	//t1.cells[1][0] = New(ConTextView, "Hello, world!");
	//t1.cells[2][0] = New(ConTextView, "Helloworld");
	//t1.FitCells();
	//t1.Update();

	
	/*ConInputTest it;
	it.Input();*/

	system("pause");
}