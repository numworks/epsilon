function InteractiveMemoryMapViewer(chartNodeSelector, legendNodeSelector) {
  let gapRemovalRatio = 0.5
  let equalizingRatio = 0.5
  let fontHeight = 10 // In pixels
  let source = undefined
  let width = 0
  let height = 0
  let svg = undefined
  let yDomAxis = undefined
  let yScale

  function NumberToHumanBytes(number) {
    if (number > 1024*1024*1024) {
      return `${Math.round(number/(1024*1024*1024))} GB`
    } else if (number > 1024*1024) {
      return `${Math.round(number/(1024*1024))} MB`
    } else if (number > 1024) {
      return `${Math.round(number/1024)} KB`
    } else {
      return `${Math.round(number)} B`
    }
  }

  function NumberToHumanAddress(number) {
    return `0x${number.toString(16).toUpperCase()}`
  }

  function compressedScaleFor(zone, gapRemovalRatio, equalizingRatio, rangeMin, rangeMax) {
    if (!zone.zones) {
      return d3.scaleLinear().domain([zone.start, zone.end]).range([rangeMin, rangeMax]);
    }
    let sortedChildren = zone.zones.sort((a, b) => (a.start > b.start) ? 1 : -1)

    /*
     * Let's say we have the following children zones:
     *
     *   ----+---+-----+-------------------------+---+-------+-------------
     *  |    | A |     |             B           |   |   C   |             |
     *   ----+---+-----+-------------------------+---+-------+-------------
     *
     * What if the gap before A or the gap after C is huge and dwarfs every other
     * area? What if B is so large that we couldn't even see A and C? To fix those
     * issues, we need to adapt the scale and make some of those areas smaller
     * than they would normally be.
     *
     * In practice, we want to make the gaps smaller (because they are pretty much
     * irrelevant). And we want to use a somewhat logarithmic scale for the zones.
     *
     * Let's compute the zone's locations if the gaps were removed:
     * - Each element would be resized to original_size* parent_size/sum_of_zone_sizes;
     * - And each element would simply start where the previous ended!
     *
     * Now what about smoothing out the size differences? Well, again the extreme
     * case is easy: every zone has the same size! Let's just use a ratio towards
     * this!
     */

    /* D3 expects a list of addresses as a domain. We'll use all the start and end
     * bounds for the main zone and its children. */

    let limits = [];
    limits.push(zone.start);
    for (let child of sortedChildren) {
      limits.push(child.start);
      limits.push(child.end);
    }
    limits.push(zone.end);


    /* Let's compute what those limits would be if:
     *  - The gaps were reduced by a gapRemovalRatio
     *  - The gaps and zones were made more equal by a equalizingRatio
     * Here's the process:
     *  - We start at zone.start */

    let sizeOfGap = function(previousEnd, newStart) {
      let size = newStart - previousEnd - 1;
      console.assert(size >= 0, "Negative gap size", size, "between", previousEnd, newStart);
      return size;
    }

    let sizeOfZone = function(start, end) {
      let size = end - start + 1;
      console.assert(size >= 0, "Negative zone size", size, "between", start, end);
      return size;
    }

    // Compute total gap and zone sizes
    let totalGapSize = 0;
    let totalNumberOfGaps = 0;
    let totalZoneSize = 0;
    let totalNumberOfZones = 0;
    let lastLimit = zone.start-1;
    for (let child of sortedChildren) {
      let gapSize = sizeOfGap(lastLimit, child.start)
      if (gapSize > 0) {
        totalNumberOfGaps++
      }
      let zoneSize = sizeOfZone(child.start, child.end)
      if (zoneSize > 0) {
        totalNumberOfZones++
      }
      totalGapSize += gapSize
      totalZoneSize += zoneSize
      lastLimit = child.end;
    }
    console.assert(lastLimit <= zone.end)
    let finalGapSize = sizeOfGap(lastLimit, zone.end)
    if (finalGapSize > 0) {
      totalGapSize += finalGapSize
      totalNumberOfGaps++
    }

    /* Let's compute what those limits would be if:
     *  - The gaps were reduced by a gapRemovalRatio
     *  - The gaps and zones were made more equal by a equalizingRatio
     * Here's the process:
     *  - We start at zone.start */

    let equalizedGapSize = totalGapSize/totalNumberOfGaps;
    let equalizedZoneSize = totalZoneSize/totalNumberOfZones;

    let weightedMean = function(w, a, b) {
      return a*(1-w) + b*w
      // return Math.exp((1-w)*Math.log(a) + w*Math.log(b));
    }

    let correctedLimits = [];
    correctedLimits.push(zone.start);
    lastLimit = zone.start-1;
    let lastCorrectedLimit = zone.start;
    let zoneSizeCorrectionRatio = (totalZoneSize + gapRemovalRatio*totalGapSize)/totalZoneSize;
    for (let child of sortedChildren) {
      let gapSize = sizeOfGap(lastLimit, child.start)
      let zoneSize = sizeOfZone(child.start, child.end)
      let correctedGapSize;
      if (gapSize > 0) {
        correctedGapSize = weightedMean(gapRemovalRatio, weightedMean(equalizingRatio, gapSize, equalizedGapSize), 0);
      } else {
        correctedGapSize = 0;
      }
      let correctedZoneSize = weightedMean(equalizingRatio, zoneSize, equalizedZoneSize);
      // Now the zone size need to be raised to make up for the removed gap
      correctedZoneSize = correctedZoneSize * zoneSizeCorrectionRatio;
      // Make up for console
      let correctedChildStart = lastCorrectedLimit + correctedGapSize;
      let correctedChildEnd = correctedChildStart + correctedZoneSize;
      correctedLimits.push(correctedChildStart);
      correctedLimits.push(correctedChildEnd);
      lastLimit = child.end;
      lastCorrectedLimit = correctedChildEnd;
    }
    correctedLimits.push(zone.end);

    let correctedRange = correctedLimits.map(address => rangeMin + (rangeMax-rangeMin)*(address-zone.start)/(zone.end-zone.start))
    console.assert(correctedRange.length == limits.length)

    return d3.scaleLinear().domain(limits).range(correctedRange);
  }

  function ticksFor(zone, scale, minTickDistance) {
    let sortedChildren = zone.zones ? zone.zones.sort((a, b) => (a.start > b.start) ? 1 : -1) : []
    let ticks = []
    function pushTick(value) {
      if (ticks.length > 0) {
        let lastTick = ticks[ticks.length-1]
        console.assert(lastTick <= value, "Overlapping zones", lastTick, value)
        if (value == lastTick+1) {
          ticks.pop()
        }
      }
      ticks.push(value)
    }
    pushTick(zone.start)
    for (let child of sortedChildren) {
      pushTick(child.start)
      pushTick(child.end)
    }
    pushTick(zone.end)

    // Let's filter out ticks that are too close to each other
    let filteredTicks = []
    let lastTick
    for (let tick of ticks) {
      if (lastTick && Math.abs(scale(lastTick)-scale(tick))<minTickDistance) {
        continue
      }
      lastTick = tick
      filteredTicks.push(tick)
    }
    return filteredTicks
  }

  function redraw(transitionDuration = 0) {
    if (!source) {
      return
    }
    yScale = compressedScaleFor(source, gapRemovalRatio, equalizingRatio, height, 0)
    let maps = svg.selectAll("g.maps");
    maps.selectAll("g.zone")
      .transition()
      .duration(transitionDuration)
      .attr("transform", function(d, i) { return `translate(0, ${yScale(d.end)})`; })
    maps
      .selectAll(".zone rect")
      .transition()
      .duration(transitionDuration)
      .attr("height", function(d) { return yScale(d.start)-yScale(d.end); })
    maps
      .selectAll(".zone text")
      .transition()
      .duration(transitionDuration)
      .attr("transform", function(d) {
        let rectHeight = yScale(d.start)-yScale(d.end)
        let transform = `translate(10,${(rectHeight+fontHeight)/2})` // Vertical centering in rect
        if (rectHeight < fontHeight) {
          transform += ' scale(1,0)'
        }
        return transform
      })

    yAxis = d3.axisLeft(yScale)
      .tickValues(ticksFor(source, yScale, 20))
      .tickFormat(NumberToHumanAddress)


    yDomAxis.transition()
      .duration(transitionDuration)
      .call(yAxis.scale(yScale));
  }

  this.setGapRemovalRatio = function(ratio, transitionDuration) {
    gapRemovalRatio = ratio
    redraw(transitionDuration)
  }

  this.setEqualizingRatio = function(ratio, transitionDuration) {
    equalizingRatio = ratio
    redraw(transitionDuration)
  }

  this.load = function(data) {
    let chartNode = document.querySelector(chartNodeSelector)
    let legendNode = d3.select(legendNodeSelector)

    // set the dimensions and margins of the graph
    let margin = {top: 30, right: 30, bottom: 70, left: 100};
    width = chartNode.scrollWidth - margin.left - margin.right
    height = chartNode.scrollHeight - margin.top - margin.bottom
    let columnMargin = 20
    let numberOfColumns = data.length
    columnWidth = (width/numberOfColumns)-columnMargin

    // append the svg object to the body of the page
    if (svg) {
      d3.select(chartNodeSelector).select("svg").remove()
    }
    svg = d3.select(chartNodeSelector)
      .append("svg")
      .attr("width", width + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
      .append("g")
      .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    let cursor = svg
      .append("rect")
      .attr("class", "cursor")
      .attr("x", "0")
      .attr("y", "0")
      .attr("width", width)
      .attr("height", "0.1")
    let cursorLegend = svg
      .append("text")
      .attr("text-anchor", "end")

    source = data[0]

    let xScale = d3.scaleLinear().domain([0,1]).range([0, width])
    yScale = d3.scaleLinear().domain([0,1]).range([0,0]) // Placeholder
    //let yScale = compressedScaleFor(source, gapRemovalRatio, equalizingRatio, 0, height)

    let yAxis = d3.axisLeft(yScale)

    yDomAxis = svg.append("g")
    //.attr("transform", "translate(0," + height + ")")
      .call(yAxis)

    // We need to create a rect to capture mouse events. We can't bind mousemove on svg
    let pointerEventCatcher = svg
      .append('rect')
      .style("fill", "none")
      .style("pointer-events", "all")
      .attr('width', width)
      .attr('height', height)

    let selectionOrigin = undefined;
    let selectionEnd = undefined;
    pointerEventCatcher
      .on("mousedown", function(e) {
        let y = Math.max(Math.min(height,d3.pointer(e)[1]),0)
        selectionOrigin = Math.round(yScale.invert(y))
      })
      .on("mouseup", function(e) {
        cursor.attr("y", yScale(selectionEnd));
        cursor.attr("height", "0.1");
        source = {start: selectionOrigin, end: selectionEnd};
        selectionOrigin = undefined;
        selectionEnd = undefined;
        redraw();
      })
    svg
      .on("mousemove", function(e) {
        let y = Math.max(Math.min(height,d3.pointer(e)[1]),0)
        selectionEnd = Math.round(yScale.invert(y))
        if (selectionOrigin) {
          cursor.attr("y", Math.min(yScale(selectionOrigin), yScale(selectionEnd)));
          cursor.attr("height", Math.abs(yScale(selectionEnd)-yScale(selectionOrigin)));
        } else {
          cursor.attr("y", y);
          cursor.attr("height", "0.1");
        }
        cursorLegend
          .attr("transform", `translate(${width}, ${y})`)
          .text(NumberToHumanAddress(selectionEnd));
      });


    let maps = svg
      .selectAll('g.maps')
      .data(data)
      .enter()
      .append('g')
      .attr("class", "maps")

    if (maps.nodes().length > 0) {
      maps.nodes()[0].classList.add("selected") // Mark first map as selected
    }

    let tabs = maps.append("g")
    tabs
      .append("rect")
      .attr("class", "label")
      .attr("width", columnWidth)
      .attr("height", "20")
      .attr("y", height+10)
    tabs
      .append("text")
      .text(d => d.name)
      .attr("transform", `translate(5,${height+25})`)
    tabs
      .on("click", function(ev, datum) {
        for (const map of this.parentElement.parentElement.querySelectorAll(".maps")) {
          map.classList.remove("selected")
        }
        this.parentElement.classList.add("selected")
        source = datum;
        redraw(300);
      })

    let zones = maps
      .attr("transform", function(d, i) { return `translate(${(columnWidth+columnMargin)*i},0)`; })
      .selectAll('g.zone')
      .data(function(d){
        return d.zones;
      })
      .enter()
      .append('g')
      .attr("class", "zone")


    zones.append("rect")
      .attr("width", columnWidth)
      .on("mouseover", function(element, datum) {
        let legend = ''
        legend += '<table>'
        legend += '  <tr>'
        legend += '    <th>Name</th>'
        legend += '    <td><code>' + datum.name + '</code></td>'
        legend += '  </tr>'
        legend += '  <tr>'
        legend += '    <th>Size</th>'
        legend += '    <td><code>' + NumberToHumanBytes(datum.end-datum.start) + ' (' + (datum.end-datum.start).toLocaleString() + ' bytes)' + '</code></td>'
        legend += '  </tr>'
        legend += '  <tr>'
        legend += '    <th>Start</th>'
        legend += '    <td><code>' + NumberToHumanAddress(datum.start) + '</code></td>'
        legend += '  </tr>'
        legend += '  <tr>'
        legend += '    <th>End</th>'
        legend += '    <td><code>' + NumberToHumanAddress(datum.end) + '</code></td>'
        legend += '  </tr>'
        legend += '</table'
        legendNode.html(legend)
      })
      .on("mouseout", function(element, datum) {
        legendNode.html("")
      })
      .on("click", function(element, datum) {
        console.log(datum)
        source = datum
        redraw(300)
      })

    zones.append("text")
      .text(d => d.name)

    redraw(0)
  }
}
