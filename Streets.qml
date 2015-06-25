import QtQuick 2.0
import Streets 1.0

StyleItem {
	id: host
	property alias model: table.model
	property alias maxWidth: table.contentWidth

	property alias cX: table.contentX
	property alias cY: table.contentY

	property string selected: ""
	property alias current: table.current
	onCurrentChanged: selected = model.nameForRow(current)
	Table {
		id: table
		anchors.fill: parent
		delegate: TableRow {
			TableCell {text: canonical; width: fontWidth*40}
			TableCell {text: secondary; width: fontWidth*10}
			TableCell {text: number; width: fontWidth*5}
			TableCell {text: type; width: fontWidth*10}
			onSelect: host.selected = wholeName
		}
		contentWidth: fontWidth*65
		header: TableHeader {
			TableHeaderCell {text: "Name"; width: fontWidth*40}
			TableHeaderCell {text: "Feat."; width: fontWidth*10}
			TableHeaderCell {text: "№"; width: fontWidth*5}
			TableHeaderCell {text: "Type"; width: fontWidth*10}
			Component.onCompleted: sort(0, false)
			onSorted: {
				var sel = selected
				current = -1
				switch(pid) {
				case 0: model.sort("canonical", dir?1:0); break;
				case 1: model.sort("secondary", dir?1:0); break;
				case 2: model.sort("number", dir?1:0); break;
				case 3: model.sort("type", dir?1:0); break;
				}
				current = model.rowForName(sel)
			}
		}
	}
}

