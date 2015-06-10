import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property alias model: table.model
	Table {
		id: table
		anchors.fill: parent
		hasSelection: false
		delegate: TableRow {
			TableCell {text: region; width: 100}
			TableCell {text: name; width: 300}
			selected: model.selected
			onSelect: table.model.select(index)
		}
		contentWidth: 400
	}
}
