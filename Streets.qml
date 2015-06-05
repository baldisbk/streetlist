import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property StreetModel model
	property string selected: ""
	Table {
		anchors.fill: parent
		model: host.model
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

