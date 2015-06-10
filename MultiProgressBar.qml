import QtQuick 2.0
import TheSystem 1.0

StyleItem {
	property int mode
	z: 100
	Rectangle {
		color: backgroundColor
		anchors.fill: parent
	}
	ListView {
		id: progressBars
		property int valHeight: textlineSize
		height: progressModel.count * valHeight
		width: parent.width - fullscreenBorder
		anchors.centerIn: parent
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
	}

	SystemFunc {id: timer}
	ListModel {id: cache}

	function clear() {progressModel.clear(); cache.clear()}
	function add(name) {
		progressModel.append({
				"pbName": name,
				"pbPercent": 0,
				"pbMax": 0,
				"pbValue": 0
			})
		cache.append({
				"max": 0,
				"value": 0,
				"chg": false
			})
	}
	function progress(ind, val, max) {
		if (ind >= progressModel.count)
			return

		cache.get(ind).max = max
		cache.get(ind).value = val
		cache.get(ind).chg = true
		if (timer.elapsedTimer() < 100)
			return;
		timer.restartTimer()

		var prg
		for (var i = 0; i < cache.count; ++i) {
			if (!cache.get(i).chg) continue

			if (cache.get(i).max === 0)
				prg = 0
			else {
				prg = cache.get(i).value * 100
				prg /= cache.get(i).max
			}
			progressModel.get(i).pbPercent = prg
			progressModel.get(i).pbValue = cache.get(i).value
			progressModel.get(i).pbMax = cache.get(i).max
			cache.get(i).chg = false
		}
	}
	onModeChanged: if (mode != 0) timer.startTimer()
	visible: mode != 0
}
