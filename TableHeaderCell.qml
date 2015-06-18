import QtQuick 2.0

StyleItem {
	property alias text: txt.text
	property bool selected: false
	property bool sortDir: false
	property int pid
	signal select(int id)
	height: textlineHeight
	onSelectedChanged: canvas.requestPaint()
	onSortDirChanged: canvas.requestPaint()
	onSelect: {
		if (!selected)
			sortDir = false;
		else
			sortDir = !sortDir
		selected = true
	}

	Rectangle {
		id: host
		gradient: Gradient {
			GradientStop { position: 0.0; color: selectedColor }
			GradientStop { position: 0.5; color: selected?selectedColor:transparentColor }
			GradientStop { position: 1.0; color: selectedColor }
		}
		border.width: headerBorder
		anchors.fill: parent
		Text {
			id: txt
			font.bold: true
			font.underline: selected
			anchors.fill: parent
			anchors.leftMargin: fontWidth / 2
			horizontalAlignment: Text.AlignLeft
			verticalAlignment: Text.AlignVCenter
		}
		Canvas {
			id: canvas
			anchors {
				top: parent.top
				right: parent.right
				topMargin: textlineHeight / 4
				rightMargin: fontWidth / 2
			}
			width: fontWidth * 1.5
			height: textlineHeight / 2
			onPaint: {
				var ctx = canvas.getContext('2d')
				ctx.save()
				ctx.clearRect(0, 0, canvas.width, canvas.height);
				if (selected) {
					var s = 2
					ctx.lineWidth = 2
					ctx.strokeStyle = "black"
					ctx.globalAlpha = 1
					ctx.beginPath();
					if (sortDir) {
						ctx.moveTo(s, s)
						ctx.lineTo(canvas.width-s, s)
						ctx.lineTo(canvas.width/2, canvas.height-s)
						ctx.lineTo(s, s)
					} else {
						ctx.moveTo(canvas.width/2, s)
						ctx.lineTo(canvas.width-s, canvas.height-s)
						ctx.lineTo(s, canvas.height-s)
						ctx.lineTo(canvas.width/2, s)
					}
					ctx.closePath();
					ctx.stroke()
				}
				ctx.restore();
			}
		}
	}
	MouseArea {
		anchors.fill: parent
		onClicked: select(pid)
	}
}
