import QtQuick 2.0

Rectangle {
	height: 20
	color: "#00000000"
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
		color: selected?"#FFFF88":"#00000000"
	}
	Row {id: row}
	Component.onCompleted: {
		var w = 0
		for (var i = 0; i < row.children.length; ++i)
			w += row.children[i].width
		width = w
	}
}
