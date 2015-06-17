import QtQuick 2.0

StyleItem {
	height: textlineHeight
	default property alias contents: row.data
	signal sorted(int pid, bool dir)
	Row {id: row}

	function onselected(pid) {
		for (var i = 0; i < row.children.length; ++i) {
			if (row.children[i].pid !== pid)
				row.children[i].selected = false
		}
		sorted(pid, row.children[pid].sortDir)
	}

	function sort(pid, dir) {
		for (var i = 0; i < row.children.length; ++i) {
			if (row.children[i].pid !== pid)
				row.children[i].selected = false
			else
				row.children[i].selected = true
		}
		row.children[pid].sortDir = dir
		sorted(pid, dir)
	}

	Component.onCompleted: {
		var w = 0
		for (var i = 0; i < row.children.length; ++i) {
			w += row.children[i].width
			row.children[i].pid = i
			row.children[i].select.connect(onselected)
		}
		width = w
	}
}
