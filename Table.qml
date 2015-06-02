import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property StreetModel model
	signal selected(string wname)
	Component {
		id: tableDelegate
		Rectangle {
			height: 20
			color: "#00000000"
			MouseArea {
				anchors.fill: parent
				onClicked: {
					list.currentIndex = index
					host.selected(wholeName)
				}
			}
			Row {
				id: row
				TableCell {text: secondary; width: 100}
				TableCell {text: name; width: 300}
				TableCell {text: number; width: 50}
				TableCell {text: type; width: 150}
			}
			Component.onCompleted: {
				var w = 0
				for (var i = 0; i < row.children.length; ++i)
					w += row.children[i].width
				width = w
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
				highlight: Component {
					id: highlightBar
					Rectangle {
						width: list.width; height: 20
						color: "#FFFF88"
					}
				}
				highlightMoveDuration: 0
			}
		}
	}
}

