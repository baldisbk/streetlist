import QtQuick 2.0
import Streets 1.0

StyleItem {
	id: host
	property alias maxWidth: table.contentWidth
	property alias model: table.model
	property alias cX: table.contentX
	property alias cY: table.contentY
	property alias current: table.current
	property string houseNum
	onCurrentChanged: houseNum = model.nameForRow(current)
	width: fontWidth * 5
	Table {
		id: table
		anchors.fill: parent
		delegate: TableRow {
			TableCell {text: name; width: fontWidth * 10}
			onSelect: host.houseNum = name
		}
		contentWidth: fontWidth * 10
	}
}
