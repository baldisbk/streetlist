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
			StyleItem {
				width: list.width
				height: textlineSize
				visible: view.hasSelection
				Rectangle {
					anchors.fill: parent
					gradient: Gradient {
						GradientStop { position: 0.0; color: selectedColor }
						GradientStop { position: 0.5; color: transparentColor }
						GradientStop { position: 1.0; color: selectedColor }
					}
				}
			}
		}
		highlightMoveDuration: 0
	}
}
