import QtQuick 2.0

Item {
	property StreetDB db
	property string filePath: fileinput.text
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Vertical
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			Button {
				text: "Save to DB"
				width: 200
				onClicked: db.todb();
			}
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			Button {
				text: "Load from DB"
				width: 200
				onClicked: db.fromdb();
			}
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			Button {
				text: "Load from WWW"
				width: 200
				onClicked: db.fromweb();
			}
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			central: inputContainer
			Button {
				text: "Load from files"
				width: 200
				onClicked: db.fromfiles(fileinput.text);
			}
			Rectangle {
				id: inputContainer
				border.width: 1
				Flickable {
					id: flick
					anchors.fill: inputContainer
					contentWidth: fileinput.paintedWidth
					contentHeight: fileinput.paintedHeight
					clip: true

					function ensureVisible(r) {
						if (contentX >= r.x)
							contentX = r.x;
						else if (contentX+width <= r.x+r.width)
							contentX = r.x+r.width-width;
						if (contentY >= r.y)
							contentY = r.y;
						else if (contentY+height <= r.y+r.height)
							contentY = r.y+r.height-height;
					}

					TextEdit {
						id: fileinput
						text: "/home/baldis/Documents/maps/maps/tmp/"
						width: flick.width
						height: flick.height
						focus: true
					//	wrapMode: TextEdit.Wrap
						verticalAlignment: TextEdit.AlignVCenter
						onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
					}
				}
			}
			Button {
				text: "..."
				width: 50
				onClicked: ;
			}
		}
		Item {id: filler}
		central: filler
	}
}

