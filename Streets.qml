import QtQuick 2.0
import Streets 1.0

Item {
	id: host
	property alias model: table.model
	property string selected: ""
	Table {
		id: table
		anchors.fill: parent
		delegate: TableRow {
			TableCell {text: secondary; width: 100}
			TableCell {text: name; width: 300}
			TableCell {text: number; width: 50}
			TableCell {text: type; width: 150}
			onSelect: host.selected = wholeName
		}
		contentWidth: 600
	}
}

