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
				gradient: Gradient {
					GradientStop { position: 0.0; color: "#FFFF88" }
//					GradientStop { position: 0.33; color: "#00000000" }
					GradientStop { position: 0.5; color: "#00000000" }
//					GradientStop { position: 0.66; color: "#00000000" }
					GradientStop { position: 1.0; color: "#FFFF88" }
				}
			}
		}
		highlightMoveDuration: 0
	}
}
