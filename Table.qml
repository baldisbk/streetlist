import QtQuick 2.4

StyleItem {
	property alias model: list.model
	property alias delegate: list.delegate
	property alias header: list.header
	property bool hasSelection: true
	property alias current: list.currentIndex
	property alias contentWidth: view.contentWidth
	Flickable {
		id: view
		anchors.fill: parent
		contentHeight: height
		clip: true
		ListView {
			id: list
			clip: true
			width: view.contentWidth
			height: view.height
			headerPositioning: ListView.PullBackHeader
			highlight: Component {
				id: highlightBar
				StyleItem {
					width: list.width
					height: textlineHeight
					visible: hasSelection
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

	Rectangle {
		color: progressColor
		anchors.right: parent.right
		width: scrollerSize
		Component.onCompleted: {
			if (list.headerItem == null)
				headHeight = 0
			else headHeight = list.headerItem.height
		}

		property int headHeight
		property int cHeight: list.contentHeight < list.height ?
			list.height - headHeight :
			list.contentHeight - headHeight
		property int cY: list.contentY + headHeight
		property int hHeight: list.height - headHeight
		property int protoY: hHeight * cY / cHeight + headHeight
		height: hHeight * hHeight / cHeight
		y: protoY < 0 ? 0 : (protoY < list.height - height ? protoY : list.height - height)
	}
}
