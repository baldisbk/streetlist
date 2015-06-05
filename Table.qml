import QtQuick 2.0

Flickable {
	id: view
	property alias model: list.model
	property alias delegate: list.delegate
	contentHeight: height
	ListView {
		id: list
		clip: true
		width: view.contentWidth
		height: view.height
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
