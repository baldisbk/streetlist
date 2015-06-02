import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property StreetModel model
	Component {
		id: tableDelegate
		Row {
			Rectangle {
				border.width: 1
				Text {
					text: secondary
					anchors.fill: parent
					horizontalAlignment: Text.AlignLeft
					verticalAlignment: Text.AlignVCenter
				}
				height: 20
				width: 100
			}
			Rectangle {
				border.width: 1
				Text {
					text: name
					anchors.fill: parent
					horizontalAlignment: Text.AlignLeft
					verticalAlignment: Text.AlignVCenter
				}
				height: 20
				width: 300
			}
			Rectangle {
				border.width: 1
				Text {
					text: number
					anchors.fill: parent
					horizontalAlignment: Text.AlignLeft
					verticalAlignment: Text.AlignVCenter
				}
				height: 20
				width: 50
			}
			Rectangle {
				border.width: 1
				Text {
					text: type
					anchors.fill: parent
					horizontalAlignment: Text.AlignLeft
					verticalAlignment: Text.AlignVCenter
				}
				height: 20
				width: 150
			}
		}
	}
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Vertical
		central: view
		Flickable {
			id: view
			contentHeight: height
			contentWidth: list.width
			ListView {
				id: list
				model: host.model
				clip: true
				width: 600
				height: view.height
				delegate: tableDelegate
			}
		}
	}
}

