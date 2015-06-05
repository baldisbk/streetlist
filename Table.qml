import QtQuick 2.0

Flickable {
	id: view
	property alias model: list.model
	property alias delegate: list.delegate
	property bool hasSelection: true
	contentHeight: height
	ListView {
		id: list
		clip: true
		width: view.contentWidth
		height: view.height
		highlight: Component {
			id: highlightBar
			Rectangle {
				visible: view.hasSelection
				width: list.width; height: 20
				gradient: Gradient {
					GradientStop { position: 0.0; color: "#FFFF88" }
					GradientStop { position: 0.5; color: "#00000000" }
					GradientStop { position: 1.0; color: "#FFFF88" }
				}
			}
		}
		highlightMoveDuration: 0
	}
}
