import QtQuick 2.0
import QtQuick.Dialogs 1.0

StyleItem {
	property StreetDB db
	property alias filePath: fileEdit.text
	FileDialog {
		id: fileopen
		selectFolder: true
		onAccepted: {
			var ind = fileUrl.toString().indexOf("://", 0)
			if (ind != -1)
				filePath = fileUrl.toString().slice(ind+3)
			else
				filePath = fileUrl.toString()
		}
	}

	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Vertical
		AutoLayout {
			orientation: Qt.Horizontal
			height: buttonSize
			Button {
				text: "Save to DB"
				width: buttonTextWidth
				onClicked: db.todb();
			}
			Item {id: filler1}
			central: filler1
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: buttonSize
			Button {
				text: "Load from DB"
				width: buttonTextWidth
				onClicked: db.fromdb();
			}
			Item {id: filler2}
			central: filler2
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: buttonSize
			Button {
				text: "Load from WWW"
				width: buttonTextWidth
				onClicked: db.fromweb();
			}
			Item {id: filler3}
			central: filler3
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: buttonSize
			central: fileEdit
			Button {
				text: "Load from files"
				width: buttonTextWidth
				onClicked: db.fromfiles(filePath);
			}
			LineEdit {id: fileEdit; text: "/home/baldis/Documents/maps/maps/tmp/"}
			Button {
				text: "..."
				onClicked: fileopen.open()
			}
		}
		Item {id: filler}
		central: filler
	}
}

