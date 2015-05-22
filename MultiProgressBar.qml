import QtQuick 2.0

ListView {
	id: progressBars
	property int mode
	property int valHeight: 20
	height: progressModel.count * valHeight
	model: ListModel {id: progressModel}
	delegate: Component {
		id: progressDelegate
		ProgressBar {
			caption: pbName
			height: progressBars.valHeight
			width: progressBars.width
			percent: pbPercent
			comment: pbValue + "/" + pbMax
		}
	}
	function clear() {progressModel.clear()}
	function add(name) {
		progressModel.append({
				"pbName": name,
				"pbPercent": 0,
				"pbMax": 0,
				"pbValue": 0
			})
	}
	function progress(ind, val, max) {
		if (ind >= progressModel.count)
			return

		var prg
		if (max === 0)
			prg = 0
		else {
			prg = val * 100
			prg /= max
		}
		progressModel.get(ind).pbPercent = prg
		progressModel.get(ind).pbValue = val
		progressModel.get(ind).pbMax = max
	}
}
