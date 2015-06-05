import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property StreetModel model
	signal selected(string wname)
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
				delegate: TableRow {
					TableCell {text: secondary; width: 100}
					TableCell {text: name; width: 300}
					TableCell {text: number; width: 50}
					TableCell {text: type; width: 150}
					onSelect: host.selected(wholeName)
				}
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

