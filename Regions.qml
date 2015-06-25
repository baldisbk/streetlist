import QtQuick 2.0
import Streets 1.0

StyleItem {
	id: host
	property alias model: table.model
	property alias maxWidth: table.contentWidth
	property alias cX: table.contentX
	property alias cY: table.contentY
	Table {
		id: table
		anchors.fill: parent
		hasSelection: false
		delegate: TableRow {
			TableCell {text: name; width: fontWidth * 10}
			selected: model.selected
			onSelect: table.model.select(index)
		}
		contentWidth: fontWidth * 10
	}
}
