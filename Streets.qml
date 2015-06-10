import QtQuick 2.0
import Streets 1.0

StyleItem {
	id: host
	property alias model: table.model
	property alias maxWidth: table.contentWidth
	property string selected: ""
	Table {
		id: table
		anchors.fill: parent
		delegate: TableRow {
			TableCell {text: secondary; width: fontSize*10}
			TableCell {text: name; width: fontSize*40}
			TableCell {text: number; width: fontSize*5}
			TableCell {text: type; width: fontSize*10}
			onSelect: host.selected = wholeName
		}
		contentWidth: fontSize*65
	}
}

