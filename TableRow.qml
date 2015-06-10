import QtQuick 2.0

StyleItem {
	height: textlineSize
	default property alias contents: row.data
	signal select()
	property bool selected: false
	onSelect: ListView.view.currentIndex = index
	MouseArea {
		anchors.fill: parent
		onClicked: select()
	}
	Rectangle {
		anchors.fill: parent
		color: selected?selectedColor:transparentColor
	}
	Row {id: row}
	Component.onCompleted: {
		var w = 0
		for (var i = 0; i < row.children.length; ++i)
			w += row.children[i].width
		width = w
	}
}
