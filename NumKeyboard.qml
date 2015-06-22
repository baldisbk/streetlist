import QtQuick 2.0

StyleItem {
	signal input(string ch)
	signal backspace()
	signal clear()
	signal enter()
	height: buttonSize * 4
	width: buttonSize * 6
	Rectangle {anchors.fill: parent; color: emptyColor}

	Column {
		Row {
			Button {text: "1"; onClicked: input("1")}
			Button {text: "2"; onClicked: input("2")}
			Button {text: "3"; onClicked: input("3")}
			Button {text: "+"; onClicked: input("+")}
			Button {text: "K"; onClicked: input("k")}
			Button {text: "X"; onClicked: input("x")}
		}
		Row {
			Button {text: "4"; onClicked: input("4")}
			Button {text: "5"; onClicked: input("5")}
			Button {text: "6"; onClicked: input("6")}
			Button {text: "-"; onClicked: input("-")}
			Button {text: "C"; onClicked: input("c")}
			Button {text: "<-"; onClicked: backspace()}
		}
		Row {
			Button {text: "7"; onClicked: input("7")}
			Button {text: "8"; onClicked: input("8")}
			Button {text: "9"; onClicked: input("9")}
			Button {text: "*"; onClicked: input("*")}
			Button {text: "B"; onClicked: input("b")}
			Button {text: "Clear"; onClicked: clear()}
		}
		Row {
			Button {text: "0"; onClicked: input("0")}
			Button {text: "("; onClicked: input("(")}
			Button {text: ")"; onClicked: input(")")}
			Button {text: "/"; onClicked: input(":")}
			Button {text: "D"; onClicked: input("/")}
			Button {text: "Enter"; onClicked: enter()}
		}
	}
}

