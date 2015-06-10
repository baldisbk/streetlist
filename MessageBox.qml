import QtQuick 2.0

StyleItem {
	id: host
	z: 150
	visible: false
	function display(txt) {thetext.text = txt; visible = true}
	Rectangle {
		color: backgroundColor
		anchors.fill: parent
	}
	Rectangle {
		height: btn.height + fontSize * 10
		width: parent.width - fullscreenBorder
		anchors.centerIn: parent
		AutoLayout {
			orientation: Qt.Vertical
			anchors.fill: parent
			central: thetext
			Text {
				id: thetext
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
			Button {
				id: btn
				text: "OK"
				onClicked: host.visible = false
			}
		}
	}
}

