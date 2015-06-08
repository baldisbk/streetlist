import QtQuick 2.0

Rectangle {
	id: host
	height: 50
	state: "brute"
	states: [
		State {
			name: "brute"
			PropertyChanges {target: bfEditor; visible: true}
			PropertyChanges {target: bfRun; visible: true}
			PropertyChanges {target: brute; checked: true}
			PropertyChanges {target: tags; checked: false}
		},
		State {
			name: "tags"
			PropertyChanges {target: brute; checked: false}
			PropertyChanges {target: tags; checked: true}
		}
	]
	AutoLayout {
		anchors.fill: host
		central: filler
		Button {
			id: brute
			image: "qrc:/pics/Clenched Fist-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "brute"
		}
		LineEdit {
			id: bfEditor
			visible: false
			width: host.width - 150
		}
		Button {
			id: bfRun
			visible: false
			width: 50
			text: "Go!"
		}
		Button {
			id: tags
			image: "qrc:/pics/Tag-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "tags"
		}

		Item {id: filler}
	}
}

