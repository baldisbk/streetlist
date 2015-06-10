import QtQuick 2.0

Item {
	id: host
	height: 50
	state: "search"
	states: [
		State {
			name: "filter"
			PropertyChanges {target: filter; checked: true}
			PropertyChanges {target: edit; checked: false}
			PropertyChanges {target: db; checked: false}
			PropertyChanges {target: search; checked: false}
			PropertyChanges {target: map; checked: false}
			PropertyChanges {target: description; checked: false}
		},
		State {
			name: "search"
			PropertyChanges {target: filter; checked: false}
			PropertyChanges {target: edit; checked: false}
			PropertyChanges {target: db; checked: false}
			PropertyChanges {target: search; checked: true}
			PropertyChanges {target: map; checked: false}
			PropertyChanges {target: description; checked: false}
		},
		State {
			name: "map"
			PropertyChanges {target: filter; checked: false}
			PropertyChanges {target: edit; checked: false}
			PropertyChanges {target: db; checked: false}
			PropertyChanges {target: search; checked: false}
			PropertyChanges {target: map; checked: true}
			PropertyChanges {target: description; checked: false}
		},
		State {
			name: "description"
			PropertyChanges {target: filter; checked: false}
			PropertyChanges {target: edit; checked: false}
			PropertyChanges {target: db; checked: false}
			PropertyChanges {target: search; checked: false}
			PropertyChanges {target: map; checked: false}
			PropertyChanges {target: description; checked: true}
		},
		State {
			name: "edit"
			PropertyChanges {target: filter; checked: false}
			PropertyChanges {target: edit; checked: true}
			PropertyChanges {target: db; checked: false}
			PropertyChanges {target: search; checked: false}
			PropertyChanges {target: map; checked: false}
			PropertyChanges {target: description; checked: false}
		},
		State {
			name: "database"
			PropertyChanges {target: filter; checked: false}
			PropertyChanges {target: edit; checked: false}
			PropertyChanges {target: db; checked: true}
			PropertyChanges {target: search; checked: false}
			PropertyChanges {target: map; checked: false}
			PropertyChanges {target: description; checked: false}
		}
	]
	Row {
		Button {
			id: db
			image: "qrc:/pics/Database-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "database"
		}
		Button {
			id: filter
			image: "qrc:/pics/Filter-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "filter"
		}
		Button {
			id: search
			image: "qrc:/pics/Search-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "search"
		}
		Button {
			id: description
			image: "qrc:/pics/List-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "description"
		}
		Button {
			id: map
			image: "qrc:/pics/Map-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "map"
		}
		Button {
			id: edit
			image: "qrc:/pics/Edit-32.png"
			text: " "
			width: 50
			height: 50
			checkable: !checked
			onCheckedChanged: if (checked) host.state = "edit"
		}
	}
}

