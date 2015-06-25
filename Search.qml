import QtQuick 2.0
import TheSystem 1.0

StyleItem {
	id: host
	property string searchString
	property alias searchedString: foundText.text
	signal search()
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
	Settings {id: settings; section: "Search"}
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
			width: host.width - buttonSize * 3 - label.width
			onEnter: search(bfEditor.text)
			onTextChanged: if (visible) searchString = text
			onVisibleChanged: if (visible) searchString = text
		}
		Rectangle {
			id: label
			width: fontWidth * 10
			Text {
				id: foundText
				anchors.fill: parent
				horizontalAlignment: Text.AlignLeft
				verticalAlignment: Text.AlignVCenter
			}
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
	function loadSettings() {
		if (settings.read("Flag") == null)
			return;
		bfEditor.text = settings.read("bfText")
		state = settings.read("state")
	}

	Component.onDestruction: {
		settings.write("Flag", 1)
		settings.write("state", state)
		settings.write("bfText", bfEditor.text)
	}
}

