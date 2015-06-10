import QtQuick 2.0

Flickable {
	id: view
	property alias model: list.model
	property alias delegate: list.delegate
	property bool hasSelection: true
	contentHeight: height
	clip: true
	ListView {
		id: list
		clip: true
		width: view.contentWidth
		height: view.height
		highlight: Component {
			id: highlightBar
			Item {
				width: list.width
				height: 20
				visible: view.hasSelection
				Rectangle {
					anchors.fill: parent
					gradient: Gradient {
						GradientStop { position: 0.0; color: "#FFFF88" }
						GradientStop { position: 0.5; color: "#00000000" }
						GradientStop { position: 1.0; color: "#FFFF88" }
					}
				}
			}
		}
		highlightMoveDuration: 0
	}
}
