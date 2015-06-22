import QtQuick 2.0

StyleItem {
	id: host
	signal search(string str)
	height: buttonSize
	state: "brute"
	z: 10
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
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "brute"
		}
		LineEdit {
			id: bfEditor
			visible: false
			editable: false
			width: host.width - buttonSize * 3
			onEnter: search(bfEditor.text)
		}
		Button {
			id: bfRun
			visible: false
			text: "Go!"
			onClicked: search(bfEditor.text)
		}
		Button {
			id: tags
			image: "qrc:/pics/Tag-32.png"
			text: " "
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "tags"
		}

		Item {id: filler}
	}
}

