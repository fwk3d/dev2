// MIT License

// Copyright (c) 2023 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.17

// Table of Contents:
//
// [SECTION] Macros and Defines
// [SECTION] Enums and Types
// [SECTION] Callbacks
// [SECTION] Contexts
// [SECTION] Begin/End Plot
// [SECTION] Begin/End Subplot
// [SECTION] Setup
// [SECTION] SetNext
// [SECTION] Plot Items
// [SECTION] Plot Tools
// [SECTION] Plot Utils
// [SECTION] Legend Utils
// [SECTION] Drag and Drop
// [SECTION] Styling
// [SECTION] Colormaps
// [SECTION] Input Mapping
// [SECTION] Miscellaneous
// [SECTION] Demo
// [SECTION] Obsolete API

#pragma once
//#include "imgui.h"

//-----------------------------------------------------------------------------
// [SECTION] Macros and Defines
//-----------------------------------------------------------------------------

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// Using ImPlot via a shared library is not recommended, because we don't guarantee
// backward nor forward ABI compatibility and also function call overhead. If you
// do use ImPlot as a DLL, be sure to call SetImGuiContext (see Miscellanous section).
#ifndef IMPLOT_API
#define IMPLOT_API
#endif

// ImPlot version string.
#define IMPLOT_VERSION "0.17"
// Indicates variable should deduced automatically.
#define IMPLOT_AUTO -1
// Special color used to indicate that a color should be deduced automatically.
#define IMPLOT_AUTO_COL ImVec4(0,0,0,-1)
// Macro for templated plotting functions; keeps header clean.
#define IMPLOT_TMP template <typename T> IMPLOT_API

//-----------------------------------------------------------------------------
// [SECTION] Enums and Types
//-----------------------------------------------------------------------------

// Forward declarations
struct ImPlotContext;             // ImPlot context (opaque struct, see implot_internal.h)

// Enums/Flags
typedef int ImAxis;                   // -> enum ImAxis_
typedef int ImPlotFlags;              // -> enum ImPlotFlags_
typedef int ImPlotAxisFlags;          // -> enum ImPlotAxisFlags_
typedef int ImPlotSubplotFlags;       // -> enum ImPlotSubplotFlags_
typedef int ImPlotLegendFlags;        // -> enum ImPlotLegendFlags_
typedef int ImPlotMouseTextFlags;     // -> enum ImPlotMouseTextFlags_
typedef int ImPlotDragToolFlags;      // -> ImPlotDragToolFlags_
typedef int ImPlotColormapScaleFlags; // -> ImPlotColormapScaleFlags_

typedef int ImPlotItemFlags;          // -> ImPlotItemFlags_
typedef int ImPlotLineFlags;          // -> ImPlotLineFlags_
typedef int ImPlotScatterFlags;       // -> ImPlotScatterFlags
typedef int ImPlotStairsFlags;        // -> ImPlotStairsFlags_
typedef int ImPlotShadedFlags;        // -> ImPlotShadedFlags_
typedef int ImPlotBarsFlags;          // -> ImPlotBarsFlags_
typedef int ImPlotBarGroupsFlags;     // -> ImPlotBarGroupsFlags_
typedef int ImPlotErrorBarsFlags;     // -> ImPlotErrorBarsFlags_
typedef int ImPlotStemsFlags;         // -> ImPlotStemsFlags_
typedef int ImPlotInfLinesFlags;      // -> ImPlotInfLinesFlags_
typedef int ImPlotPieChartFlags;      // -> ImPlotPieChartFlags_
typedef int ImPlotHeatmapFlags;       // -> ImPlotHeatmapFlags_
typedef int ImPlotHistogramFlags;     // -> ImPlotHistogramFlags_
typedef int ImPlotDigitalFlags;       // -> ImPlotDigitalFlags_
typedef int ImPlotImageFlags;         // -> ImPlotImageFlags_
typedef int ImPlotTextFlags;          // -> ImPlotTextFlags_
typedef int ImPlotDummyFlags;         // -> ImPlotDummyFlags_

typedef int ImPlotCond;               // -> enum ImPlotCond_
typedef int ImPlotCol;                // -> enum ImPlotCol_
typedef int ImPlotStyleVar;           // -> enum ImPlotStyleVar_
typedef int ImPlotScale;              // -> enum ImPlotScale_
typedef int ImPlotMarker;             // -> enum ImPlotMarker_
typedef int ImPlotColormap;           // -> enum ImPlotColormap_
typedef int ImPlotLocation;           // -> enum ImPlotLocation_
typedef int ImPlotBin;                // -> enum ImPlotBin_

// Axis indices. The values assigned may change; NEVER hardcode these.
enum ImAxis_ {
    // horizontal axes
    ImAxis_X1 = 0, // enabled by default
    ImAxis_X2,     // disabled by default
    ImAxis_X3,     // disabled by default
    // vertical axes
    ImAxis_Y1,     // enabled by default
    ImAxis_Y2,     // disabled by default
    ImAxis_Y3,     // disabled by default
    // bookeeping
    ImAxis_COUNT
};

// Options for plots (see BeginPlot).
enum ImPlotFlags_ {
    ImPlotFlags_None          = 0,       // default
    ImPlotFlags_NoTitle       = 1 << 0,  // the plot title will not be displayed (titles are also hidden if preceeded by double hashes, e.g. "##MyPlot")
    ImPlotFlags_NoLegend      = 1 << 1,  // the legend will not be displayed
    ImPlotFlags_NoMouseText   = 1 << 2,  // the mouse position, in plot coordinates, will not be displayed inside of the plot
    ImPlotFlags_NoInputs      = 1 << 3,  // the user will not be able to interact with the plot
    ImPlotFlags_NoMenus       = 1 << 4,  // the user will not be able to open context menus
    ImPlotFlags_NoBoxSelect   = 1 << 5,  // the user will not be able to box-select
    ImPlotFlags_NoFrame       = 1 << 6,  // the ImGui frame will not be rendered
    ImPlotFlags_Equal         = 1 << 7,  // x and y axes pairs will be constrained to have the same units/pixel
    ImPlotFlags_Crosshairs    = 1 << 8,  // the default mouse cursor will be replaced with a crosshair when hovered
    ImPlotFlags_CanvasOnly    = ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMouseText
};

// Options for plot axes (see SetupAxis).
enum ImPlotAxisFlags_ {
    ImPlotAxisFlags_None          = 0,       // default
    ImPlotAxisFlags_NoLabel       = 1 << 0,  // the axis label will not be displayed (axis labels are also hidden if the supplied string name is nullptr)
    ImPlotAxisFlags_NoGridLines   = 1 << 1,  // no grid lines will be displayed
    ImPlotAxisFlags_NoTickMarks   = 1 << 2,  // no tick marks will be displayed
    ImPlotAxisFlags_NoTickLabels  = 1 << 3,  // no text labels will be displayed
    ImPlotAxisFlags_NoInitialFit  = 1 << 4,  // axis will not be initially fit to data extents on the first rendered frame
    ImPlotAxisFlags_NoMenus       = 1 << 5,  // the user will not be able to open context menus with right-click
    ImPlotAxisFlags_NoSideSwitch  = 1 << 6,  // the user will not be able to switch the axis side by dragging it
    ImPlotAxisFlags_NoHighlight   = 1 << 7,  // the axis will not have its background highlighted when hovered or held
    ImPlotAxisFlags_Opposite      = 1 << 8,  // axis ticks and labels will be rendered on the conventionally opposite side (i.e, right or top)
    ImPlotAxisFlags_Foreground    = 1 << 9,  // grid lines will be displayed in the foreground (i.e. on top of data) instead of the background
    ImPlotAxisFlags_Invert        = 1 << 10, // the axis will be inverted
    ImPlotAxisFlags_AutoFit       = 1 << 11, // axis will be auto-fitting to data extents
    ImPlotAxisFlags_RangeFit      = 1 << 12, // axis will only fit points if the point is in the visible range of the **orthogonal** axis
    ImPlotAxisFlags_PanStretch    = 1 << 13, // panning in a locked or constrained state will cause the axis to stretch if possible
    ImPlotAxisFlags_LockMin       = 1 << 14, // the axis minimum value will be locked when panning/zooming
    ImPlotAxisFlags_LockMax       = 1 << 15, // the axis maximum value will be locked when panning/zooming
    ImPlotAxisFlags_Lock          = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax,
    ImPlotAxisFlags_NoDecorations = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels,
    ImPlotAxisFlags_AuxDefault    = ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Opposite
};

// Options for subplots (see BeginSubplot)
enum ImPlotSubplotFlags_ {
    ImPlotSubplotFlags_None        = 0,       // default
    ImPlotSubplotFlags_NoTitle     = 1 << 0,  // the subplot title will not be displayed (titles are also hidden if preceeded by double hashes, e.g. "##MySubplot")
    ImPlotSubplotFlags_NoLegend    = 1 << 1,  // the legend will not be displayed (only applicable if ImPlotSubplotFlags_ShareItems is enabled)
    ImPlotSubplotFlags_NoMenus     = 1 << 2,  // the user will not be able to open context menus with right-click
    ImPlotSubplotFlags_NoResize    = 1 << 3,  // resize splitters between subplot cells will be not be provided
    ImPlotSubplotFlags_NoAlign     = 1 << 4,  // subplot edges will not be aligned vertically or horizontally
    ImPlotSubplotFlags_ShareItems  = 1 << 5,  // items across all subplots will be shared and rendered into a single legend entry
    ImPlotSubplotFlags_LinkRows    = 1 << 6,  // link the y-axis limits of all plots in each row (does not apply to auxiliary axes)
    ImPlotSubplotFlags_LinkCols    = 1 << 7,  // link the x-axis limits of all plots in each column (does not apply to auxiliary axes)
    ImPlotSubplotFlags_LinkAllX    = 1 << 8,  // link the x-axis limits in every plot in the subplot (does not apply to auxiliary axes)
    ImPlotSubplotFlags_LinkAllY    = 1 << 9,  // link the y-axis limits in every plot in the subplot (does not apply to auxiliary axes)
    ImPlotSubplotFlags_ColMajor    = 1 << 10  // subplots are added in column major order instead of the default row major order
};

// Options for legends (see SetupLegend)
enum ImPlotLegendFlags_ {
    ImPlotLegendFlags_None            = 0,      // default
    ImPlotLegendFlags_NoButtons       = 1 << 0, // legend icons will not function as hide/show buttons
    ImPlotLegendFlags_NoHighlightItem = 1 << 1, // plot items will not be highlighted when their legend entry is hovered
    ImPlotLegendFlags_NoHighlightAxis = 1 << 2, // axes will not be highlighted when legend entries are hovered (only relevant if x/y-axis count > 1)
    ImPlotLegendFlags_NoMenus         = 1 << 3, // the user will not be able to open context menus with right-click
    ImPlotLegendFlags_Outside         = 1 << 4, // legend will be rendered outside of the plot area
    ImPlotLegendFlags_Horizontal      = 1 << 5, // legend entries will be displayed horizontally
    ImPlotLegendFlags_Sort            = 1 << 6, // legend entries will be displayed in alphabetical order
};

// Options for mouse hover text (see SetupMouseText)
enum ImPlotMouseTextFlags_ {
    ImPlotMouseTextFlags_None        = 0,      // default
    ImPlotMouseTextFlags_NoAuxAxes   = 1 << 0, // only show the mouse position for primary axes
    ImPlotMouseTextFlags_NoFormat    = 1 << 1, // axes label formatters won't be used to render text
    ImPlotMouseTextFlags_ShowAlways  = 1 << 2, // always display mouse position even if plot not hovered
};

// Options for DragPoint, DragLine, DragRect
enum ImPlotDragToolFlags_ {
    ImPlotDragToolFlags_None      = 0,      // default
    ImPlotDragToolFlags_NoCursors = 1 << 0, // drag tools won't change cursor icons when hovered or held
    ImPlotDragToolFlags_NoFit     = 1 << 1, // the drag tool won't be considered for plot fits
    ImPlotDragToolFlags_NoInputs  = 1 << 2, // lock the tool from user inputs
    ImPlotDragToolFlags_Delayed   = 1 << 3, // tool rendering will be delayed one frame; useful when applying position-constraints
};

// Flags for ColormapScale
enum ImPlotColormapScaleFlags_ {
    ImPlotColormapScaleFlags_None     = 0,      // default
    ImPlotColormapScaleFlags_NoLabel  = 1 << 0, // the colormap axis label will not be displayed
    ImPlotColormapScaleFlags_Opposite = 1 << 1, // render the colormap label and tick labels on the opposite side
    ImPlotColormapScaleFlags_Invert   = 1 << 2, // invert the colormap bar and axis scale (this only affects rendering; if you only want to reverse the scale mapping, make scale_min > scale_max)
};

// Flags for ANY PlotX function
enum ImPlotItemFlags_ {
    ImPlotItemFlags_None     = 0,
    ImPlotItemFlags_NoLegend = 1 << 0, // the item won't have a legend entry displayed
    ImPlotItemFlags_NoFit    = 1 << 1, // the item won't be considered for plot fits
};

// Flags for PlotLine
enum ImPlotLineFlags_ {
    ImPlotLineFlags_None        = 0,       // default
    ImPlotLineFlags_Segments    = 1 << 10, // a line segment will be rendered from every two consecutive points
    ImPlotLineFlags_Loop        = 1 << 11, // the last and first point will be connected to form a closed loop
    ImPlotLineFlags_SkipNaN     = 1 << 12, // NaNs values will be skipped instead of rendered as missing data
    ImPlotLineFlags_NoClip      = 1 << 13, // markers (if displayed) on the edge of a plot will not be clipped
    ImPlotLineFlags_Shaded      = 1 << 14, // a filled region between the line and horizontal origin will be rendered; use PlotShaded for more advanced cases
};

// Flags for PlotScatter
enum ImPlotScatterFlags_ {
    ImPlotScatterFlags_None   = 0,       // default
    ImPlotScatterFlags_NoClip = 1 << 10, // markers on the edge of a plot will not be clipped
};

// Flags for PlotStairs
enum ImPlotStairsFlags_ {
    ImPlotStairsFlags_None     = 0,       // default
    ImPlotStairsFlags_PreStep  = 1 << 10, // the y value is continued constantly to the left from every x position, i.e. the interval (x[i-1], x[i]] has the value y[i]
    ImPlotStairsFlags_Shaded   = 1 << 11  // a filled region between the stairs and horizontal origin will be rendered; use PlotShaded for more advanced cases
};

// Flags for PlotShaded (placeholder)
enum ImPlotShadedFlags_ {
    ImPlotShadedFlags_None  = 0 // default
};

// Flags for PlotBars
enum ImPlotBarsFlags_ {
    ImPlotBarsFlags_None         = 0,       // default
    ImPlotBarsFlags_Horizontal   = 1 << 10, // bars will be rendered horizontally on the current y-axis
};

// Flags for PlotBarGroups
enum ImPlotBarGroupsFlags_ {
    ImPlotBarGroupsFlags_None        = 0,       // default
    ImPlotBarGroupsFlags_Horizontal  = 1 << 10, // bar groups will be rendered horizontally on the current y-axis
    ImPlotBarGroupsFlags_Stacked     = 1 << 11, // items in a group will be stacked on top of each other
};

// Flags for PlotErrorBars
enum ImPlotErrorBarsFlags_ {
    ImPlotErrorBarsFlags_None       = 0,       // default
    ImPlotErrorBarsFlags_Horizontal = 1 << 10, // error bars will be rendered horizontally on the current y-axis
};

// Flags for PlotStems
enum ImPlotStemsFlags_ {
    ImPlotStemsFlags_None       = 0,       // default
    ImPlotStemsFlags_Horizontal = 1 << 10, // stems will be rendered horizontally on the current y-axis
};

// Flags for PlotInfLines
enum ImPlotInfLinesFlags_ {
    ImPlotInfLinesFlags_None       = 0,      // default
    ImPlotInfLinesFlags_Horizontal = 1 << 10 // lines will be rendered horizontally on the current y-axis
};

// Flags for PlotPieChart
enum ImPlotPieChartFlags_ {
    ImPlotPieChartFlags_None         = 0,       // default
    ImPlotPieChartFlags_Normalize    = 1 << 10, // force normalization of pie chart values (i.e. always make a full circle if sum < 0)
    ImPlotPieChartFlags_IgnoreHidden = 1 << 11  // ignore hidden slices when drawing the pie chart (as if they were not there)
};

// Flags for PlotHeatmap
enum ImPlotHeatmapFlags_ {
    ImPlotHeatmapFlags_None     = 0,       // default
    ImPlotHeatmapFlags_ColMajor = 1 << 10, // data will be read in column major order
};

// Flags for PlotHistogram and PlotHistogram2D
enum ImPlotHistogramFlags_ {
    ImPlotHistogramFlags_None       = 0,       // default
    ImPlotHistogramFlags_Horizontal = 1 << 10, // histogram bars will be rendered horizontally (not supported by PlotHistogram2D)
    ImPlotHistogramFlags_Cumulative = 1 << 11, // each bin will contain its count plus the counts of all previous bins (not supported by PlotHistogram2D)
    ImPlotHistogramFlags_Density    = 1 << 12, // counts will be normalized, i.e. the PDF will be visualized, or the CDF will be visualized if Cumulative is also set
    ImPlotHistogramFlags_NoOutliers = 1 << 13, // exclude values outside the specifed histogram range from the count toward normalizing and cumulative counts
    ImPlotHistogramFlags_ColMajor   = 1 << 14  // data will be read in column major order (not supported by PlotHistogram)
};

// Flags for PlotDigital (placeholder)
enum ImPlotDigitalFlags_ {
    ImPlotDigitalFlags_None = 0 // default
};

// Flags for PlotImage (placeholder)
enum ImPlotImageFlags_ {
    ImPlotImageFlags_None = 0 // default
};

// Flags for PlotText
enum ImPlotTextFlags_ {
    ImPlotTextFlags_None     = 0,       // default
    ImPlotTextFlags_Vertical = 1 << 10  // text will be rendered vertically
};

// Flags for PlotDummy (placeholder)
enum ImPlotDummyFlags_ {
    ImPlotDummyFlags_None = 0 // default
};

// Represents a condition for SetupAxisLimits etc. (same as ImGuiCond, but we only support a subset of those enums)
enum ImPlotCond_
{
    ImPlotCond_None   = ImGuiCond_None,    // No condition (always set the variable), same as _Always
    ImPlotCond_Always = ImGuiCond_Always,  // No condition (always set the variable)
    ImPlotCond_Once   = ImGuiCond_Once,    // Set the variable once per runtime session (only the first call will succeed)
};

// Plot styling colors.
enum ImPlotCol_ {
    // item styling colors
    ImPlotCol_Line,          // plot line/outline color (defaults to next unused color in current colormap)
    ImPlotCol_Fill,          // plot fill color for bars (defaults to the current line color)
    ImPlotCol_MarkerOutline, // marker outline color (defaults to the current line color)
    ImPlotCol_MarkerFill,    // marker fill color (defaults to the current line color)
    ImPlotCol_ErrorBar,      // error bar color (defaults to ImGuiCol_Text)
    // plot styling colors
    ImPlotCol_FrameBg,       // plot frame background color (defaults to ImGuiCol_FrameBg)
    ImPlotCol_PlotBg,        // plot area background color (defaults to ImGuiCol_WindowBg)
    ImPlotCol_PlotBorder,    // plot area border color (defaults to ImGuiCol_Border)
    ImPlotCol_LegendBg,      // legend background color (defaults to ImGuiCol_PopupBg)
    ImPlotCol_LegendBorder,  // legend border color (defaults to ImPlotCol_PlotBorder)
    ImPlotCol_LegendText,    // legend text color (defaults to ImPlotCol_InlayText)
    ImPlotCol_TitleText,     // plot title text color (defaults to ImGuiCol_Text)
    ImPlotCol_InlayText,     // color of text appearing inside of plots (defaults to ImGuiCol_Text)
    ImPlotCol_AxisText,      // axis label and tick lables color (defaults to ImGuiCol_Text)
    ImPlotCol_AxisGrid,      // axis grid color (defaults to 25% ImPlotCol_AxisText)
    ImPlotCol_AxisTick,      // axis tick color (defaults to AxisGrid)
    ImPlotCol_AxisBg,        // background color of axis hover region (defaults to transparent)
    ImPlotCol_AxisBgHovered, // axis hover color (defaults to ImGuiCol_ButtonHovered)
    ImPlotCol_AxisBgActive,  // axis active color (defaults to ImGuiCol_ButtonActive)
    ImPlotCol_Selection,     // box-selection color (defaults to yellow)
    ImPlotCol_Crosshairs,    // crosshairs color (defaults to ImPlotCol_PlotBorder)
    ImPlotCol_COUNT
};

// Plot styling variables.
enum ImPlotStyleVar_ {
    // item styling variables
    ImPlotStyleVar_LineWeight,         // float,  plot item line weight in pixels
    ImPlotStyleVar_Marker,             // int,    marker specification
    ImPlotStyleVar_MarkerSize,         // float,  marker size in pixels (roughly the marker's "radius")
    ImPlotStyleVar_MarkerWeight,       // float,  plot outline weight of markers in pixels
    ImPlotStyleVar_FillAlpha,          // float,  alpha modifier applied to all plot item fills
    ImPlotStyleVar_ErrorBarSize,       // float,  error bar whisker width in pixels
    ImPlotStyleVar_ErrorBarWeight,     // float,  error bar whisker weight in pixels
    ImPlotStyleVar_DigitalBitHeight,   // float,  digital channels bit height (at 1) in pixels
    ImPlotStyleVar_DigitalBitGap,      // float,  digital channels bit padding gap in pixels
    // plot styling variables
    ImPlotStyleVar_PlotBorderSize,     // float,  thickness of border around plot area
    ImPlotStyleVar_MinorAlpha,         // float,  alpha multiplier applied to minor axis grid lines
    ImPlotStyleVar_MajorTickLen,       // ImVec2, major tick lengths for X and Y axes
    ImPlotStyleVar_MinorTickLen,       // ImVec2, minor tick lengths for X and Y axes
    ImPlotStyleVar_MajorTickSize,      // ImVec2, line thickness of major ticks
    ImPlotStyleVar_MinorTickSize,      // ImVec2, line thickness of minor ticks
    ImPlotStyleVar_MajorGridSize,      // ImVec2, line thickness of major grid lines
    ImPlotStyleVar_MinorGridSize,      // ImVec2, line thickness of minor grid lines
    ImPlotStyleVar_PlotPadding,        // ImVec2, padding between widget frame and plot area, labels, or outside legends (i.e. main padding)
    ImPlotStyleVar_LabelPadding,       // ImVec2, padding between axes labels, tick labels, and plot edge
    ImPlotStyleVar_LegendPadding,      // ImVec2, legend padding from plot edges
    ImPlotStyleVar_LegendInnerPadding, // ImVec2, legend inner padding from legend edges
    ImPlotStyleVar_LegendSpacing,      // ImVec2, spacing between legend entries
    ImPlotStyleVar_MousePosPadding,    // ImVec2, padding between plot edge and interior info text
    ImPlotStyleVar_AnnotationPadding,  // ImVec2, text padding around annotation labels
    ImPlotStyleVar_FitPadding,         // ImVec2, additional fit padding as a percentage of the fit extents (e.g. ImVec2(0.1f,0.1f) adds 10% to the fit extents of X and Y)
    ImPlotStyleVar_PlotDefaultSize,    // ImVec2, default size used when ImVec2(0,0) is passed to BeginPlot
    ImPlotStyleVar_PlotMinSize,        // ImVec2, minimum size plot frame can be when shrunk
    ImPlotStyleVar_COUNT
};

// Axis scale
enum ImPlotScale_ {
    ImPlotScale_Linear = 0, // default linear scale
    ImPlotScale_Time,       // date/time scale
    ImPlotScale_Log10,      // base 10 logartithmic scale
    ImPlotScale_SymLog,     // symmetric log scale
};

// Marker specifications.
enum ImPlotMarker_ {
    ImPlotMarker_None = -1, // no marker
    ImPlotMarker_Circle,    // a circle marker (default)
    ImPlotMarker_Square,    // a square maker
    ImPlotMarker_Diamond,   // a diamond marker
    ImPlotMarker_Up,        // an upward-pointing triangle marker
    ImPlotMarker_Down,      // an downward-pointing triangle marker
    ImPlotMarker_Left,      // an leftward-pointing triangle marker
    ImPlotMarker_Right,     // an rightward-pointing triangle marker
    ImPlotMarker_Cross,     // a cross marker (not fillable)
    ImPlotMarker_Plus,      // a plus marker (not fillable)
    ImPlotMarker_Asterisk,  // a asterisk marker (not fillable)
    ImPlotMarker_COUNT
};

// Built-in colormaps
enum ImPlotColormap_ {
    ImPlotColormap_Deep     = 0,   // a.k.a. seaborn deep             (qual=true,  n=10) (default)
    ImPlotColormap_Dark     = 1,   // a.k.a. matplotlib "Set1"        (qual=true,  n=9 )
    ImPlotColormap_Pastel   = 2,   // a.k.a. matplotlib "Pastel1"     (qual=true,  n=9 )
    ImPlotColormap_Paired   = 3,   // a.k.a. matplotlib "Paired"      (qual=true,  n=12)
    ImPlotColormap_Viridis  = 4,   // a.k.a. matplotlib "viridis"     (qual=false, n=11)
    ImPlotColormap_Plasma   = 5,   // a.k.a. matplotlib "plasma"      (qual=false, n=11)
    ImPlotColormap_Hot      = 6,   // a.k.a. matplotlib/MATLAB "hot"  (qual=false, n=11)
    ImPlotColormap_Cool     = 7,   // a.k.a. matplotlib/MATLAB "cool" (qual=false, n=11)
    ImPlotColormap_Pink     = 8,   // a.k.a. matplotlib/MATLAB "pink" (qual=false, n=11)
    ImPlotColormap_Jet      = 9,   // a.k.a. MATLAB "jet"             (qual=false, n=11)
    ImPlotColormap_Twilight = 10,  // a.k.a. matplotlib "twilight"    (qual=false, n=11)
    ImPlotColormap_RdBu     = 11,  // red/blue, Color Brewer          (qual=false, n=11)
    ImPlotColormap_BrBG     = 12,  // brown/blue-green, Color Brewer  (qual=false, n=11)
    ImPlotColormap_PiYG     = 13,  // pink/yellow-green, Color Brewer (qual=false, n=11)
    ImPlotColormap_Spectral = 14,  // color spectrum, Color Brewer    (qual=false, n=11)
    ImPlotColormap_Greys    = 15,  // white/black                     (qual=false, n=2 )
};

// Used to position items on a plot (e.g. legends, labels, etc.)
enum ImPlotLocation_ {
    ImPlotLocation_Center    = 0,                                          // center-center
    ImPlotLocation_North     = 1 << 0,                                     // top-center
    ImPlotLocation_South     = 1 << 1,                                     // bottom-center
    ImPlotLocation_West      = 1 << 2,                                     // center-left
    ImPlotLocation_East      = 1 << 3,                                     // center-right
    ImPlotLocation_NorthWest = ImPlotLocation_North | ImPlotLocation_West, // top-left
    ImPlotLocation_NorthEast = ImPlotLocation_North | ImPlotLocation_East, // top-right
    ImPlotLocation_SouthWest = ImPlotLocation_South | ImPlotLocation_West, // bottom-left
    ImPlotLocation_SouthEast = ImPlotLocation_South | ImPlotLocation_East  // bottom-right
};

// Enums for different automatic histogram binning methods (k = bin count or w = bin width)
enum ImPlotBin_ {
    ImPlotBin_Sqrt    = -1, // k = sqrt(n)
    ImPlotBin_Sturges = -2, // k = 1 + log2(n)
    ImPlotBin_Rice    = -3, // k = 2 * cbrt(n)
    ImPlotBin_Scott   = -4, // w = 3.49 * sigma / cbrt(n)
};

// Double precision version of ImVec2 used by ImPlot. Extensible by end users.
IM_MSVC_RUNTIME_CHECKS_OFF
struct ImPlotPoint {
    double x, y;
    constexpr ImPlotPoint()                     : x(0.0), y(0.0) { }
    constexpr ImPlotPoint(double _x, double _y) : x(_x), y(_y) { }
    constexpr ImPlotPoint(const ImVec2& p)      : x((double)p.x), y((double)p.y) { }
    double& operator[] (size_t idx)             { IM_ASSERT(idx == 0 || idx == 1); return ((double*)(void*)(char*)this)[idx]; }
    double  operator[] (size_t idx) const       { IM_ASSERT(idx == 0 || idx == 1); return ((const double*)(const void*)(const char*)this)[idx]; }
#ifdef IMPLOT_POINT_CLASS_EXTRA
    IMPLOT_POINT_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h
                                 // to convert back and forth between your math types and ImPlotPoint.
#endif
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Range defined by a min/max value.
struct ImPlotRange {
    double Min, Max;
    constexpr ImPlotRange()                         : Min(0.0), Max(0.0) { }
    constexpr ImPlotRange(double _min, double _max) : Min(_min), Max(_max) { }
    bool Contains(double value) const               { return value >= Min && value <= Max;                      }
    double Size() const                             { return Max - Min;                                         }
    double Clamp(double value) const                { return (value < Min) ? Min : (value > Max) ? Max : value; }
};

// Combination of two range limits for X and Y axes. Also an AABB defined by Min()/Max().
struct ImPlotRect {
    ImPlotRange X, Y;
    constexpr ImPlotRect()                                                       : X(0.0,0.0), Y(0.0,0.0) { }
    constexpr ImPlotRect(double x_min, double x_max, double y_min, double y_max) : X(x_min, x_max), Y(y_min, y_max) { }
    bool Contains(const ImPlotPoint& p) const                                    { return Contains(p.x, p.y);                 }
    bool Contains(double x, double y) const                                      { return X.Contains(x) && Y.Contains(y);     }
    ImPlotPoint Size() const                                                     { return ImPlotPoint(X.Size(), Y.Size());    }
    ImPlotPoint Clamp(const ImPlotPoint& p)                                      { return Clamp(p.x, p.y);                    }
    ImPlotPoint Clamp(double x, double y)                                        { return ImPlotPoint(X.Clamp(x),Y.Clamp(y)); }
    ImPlotPoint Min() const                                                      { return ImPlotPoint(X.Min, Y.Min);          }
    ImPlotPoint Max() const                                                      { return ImPlotPoint(X.Max, Y.Max);          }
};

// Plot style structure
struct ImPlotStyle {
    // item styling variables
    float   LineWeight;              // = 1,      item line weight in pixels
    int     Marker;                  // = ImPlotMarker_None, marker specification
    float   MarkerSize;              // = 4,      marker size in pixels (roughly the marker's "radius")
    float   MarkerWeight;            // = 1,      outline weight of markers in pixels
    float   FillAlpha;               // = 1,      alpha modifier applied to plot fills
    float   ErrorBarSize;            // = 5,      error bar whisker width in pixels
    float   ErrorBarWeight;          // = 1.5,    error bar whisker weight in pixels
    float   DigitalBitHeight;        // = 8,      digital channels bit height (at y = 1.0f) in pixels
    float   DigitalBitGap;           // = 4,      digital channels bit padding gap in pixels
    // plot styling variables
    float   PlotBorderSize;          // = 1,      line thickness of border around plot area
    float   MinorAlpha;              // = 0.25    alpha multiplier applied to minor axis grid lines
    ImVec2  MajorTickLen;            // = 10,10   major tick lengths for X and Y axes
    ImVec2  MinorTickLen;            // = 5,5     minor tick lengths for X and Y axes
    ImVec2  MajorTickSize;           // = 1,1     line thickness of major ticks
    ImVec2  MinorTickSize;           // = 1,1     line thickness of minor ticks
    ImVec2  MajorGridSize;           // = 1,1     line thickness of major grid lines
    ImVec2  MinorGridSize;           // = 1,1     line thickness of minor grid lines
    ImVec2  PlotPadding;             // = 10,10   padding between widget frame and plot area, labels, or outside legends (i.e. main padding)
    ImVec2  LabelPadding;            // = 5,5     padding between axes labels, tick labels, and plot edge
    ImVec2  LegendPadding;           // = 10,10   legend padding from plot edges
    ImVec2  LegendInnerPadding;      // = 5,5     legend inner padding from legend edges
    ImVec2  LegendSpacing;           // = 5,0     spacing between legend entries
    ImVec2  MousePosPadding;         // = 10,10   padding between plot edge and interior mouse location text
    ImVec2  AnnotationPadding;       // = 2,2     text padding around annotation labels
    ImVec2  FitPadding;              // = 0,0     additional fit padding as a percentage of the fit extents (e.g. ImVec2(0.1f,0.1f) adds 10% to the fit extents of X and Y)
    ImVec2  PlotDefaultSize;         // = 400,300 default size used when ImVec2(0,0) is passed to BeginPlot
    ImVec2  PlotMinSize;             // = 200,150 minimum size plot frame can be when shrunk
    // style colors
    ImVec4  Colors[ImPlotCol_COUNT]; // Array of styling colors. Indexable with ImPlotCol_ enums.
    // colormap
    ImPlotColormap Colormap;         // The current colormap. Set this to either an ImPlotColormap_ enum or an index returned by AddColormap.
    // settings/flags
    bool    UseLocalTime;            // = false,  axis labels will be formatted for your timezone when ImPlotAxisFlag_Time is enabled
    bool    UseISO8601;              // = false,  dates will be formatted according to ISO 8601 where applicable (e.g. YYYY-MM-DD, YYYY-MM, --MM-DD, etc.)
    bool    Use24HourClock;          // = false,  times will be formatted using a 24 hour clock
    IMPLOT_API ImPlotStyle();
};

// Support for legacy versions
#if (IMGUI_VERSION_NUM < 18716) // Renamed in 1.88
#define ImGuiMod_None       0
#define ImGuiMod_Ctrl       ImGuiKeyModFlags_Ctrl
#define ImGuiMod_Shift      ImGuiKeyModFlags_Shift
#define ImGuiMod_Alt        ImGuiKeyModFlags_Alt
#define ImGuiMod_Super      ImGuiKeyModFlags_Super
#elif (IMGUI_VERSION_NUM < 18823) // Renamed in 1.89, sorry
#define ImGuiMod_None       0
#define ImGuiMod_Ctrl       ImGuiModFlags_Ctrl
#define ImGuiMod_Shift      ImGuiModFlags_Shift
#define ImGuiMod_Alt        ImGuiModFlags_Alt
#define ImGuiMod_Super      ImGuiModFlags_Super
#endif

// Input mapping structure. Default values listed. See also MapInputDefault, MapInputReverse.
struct ImPlotInputMap {
    ImGuiMouseButton Pan;           // LMB    enables panning when held,
    int              PanMod;        // none   optional modifier that must be held for panning/fitting
    ImGuiMouseButton Fit;           // LMB    initiates fit when double clicked
    ImGuiMouseButton Select;        // RMB    begins box selection when pressed and confirms selection when released
    ImGuiMouseButton SelectCancel;  // LMB    cancels active box selection when pressed; cannot be same as Select
    int              SelectMod;     // none   optional modifier that must be held for box selection
    int              SelectHorzMod; // Alt    expands active box selection horizontally to plot edge when held
    int              SelectVertMod; // Shift  expands active box selection vertically to plot edge when held
    ImGuiMouseButton Menu;          // RMB    opens context menus (if enabled) when clicked
    int              OverrideMod;   // Ctrl   when held, all input is ignored; used to enable axis/plots as DND sources
    int              ZoomMod;       // none   optional modifier that must be held for scroll wheel zooming
    float            ZoomRate;      // 0.1f   zoom rate for scroll (e.g. 0.1f = 10% plot range every scroll click); make negative to invert
    IMPLOT_API ImPlotInputMap();
};

//-----------------------------------------------------------------------------
// [SECTION] Callbacks
//-----------------------------------------------------------------------------

// Callback signature for axis tick label formatter.
typedef int (*ImPlotFormatter)(double value, char* buff, int size, void* user_data);

// Callback signature for data getter.
typedef ImPlotPoint (*ImPlotGetter)(int idx, void* user_data);

// Callback signature for axis transform.
typedef double (*ImPlotTransform)(double value, void* user_data);

namespace ImPlot {

//-----------------------------------------------------------------------------
// [SECTION] Contexts
//-----------------------------------------------------------------------------

// Creates a new ImPlot context. Call this after ImGui::CreateContext.
IMPLOT_API ImPlotContext* CreateContext();
// Destroys an ImPlot context. Call this before ImGui::DestroyContext. nullptr = destroy current context.
IMPLOT_API void DestroyContext(ImPlotContext* ctx = nullptr);
// Returns the current ImPlot context. nullptr if no context has ben set.
IMPLOT_API ImPlotContext* GetCurrentContext();
// Sets the current ImPlot context.
IMPLOT_API void SetCurrentContext(ImPlotContext* ctx);

// Sets the current **ImGui** context. This is ONLY necessary if you are compiling
// ImPlot as a DLL (not recommended) separate from your ImGui compilation. It
// sets the global variable GImGui, which is not shared across DLL boundaries.
// See GImGui documentation in imgui.cpp for more details.
IMPLOT_API void SetImGuiContext(ImGuiContext* ctx);

//-----------------------------------------------------------------------------
// [SECTION] Begin/End Plot
//-----------------------------------------------------------------------------

// Starts a 2D plotting context. If this function returns true, EndPlot() MUST
// be called! You are encouraged to use the following convention:
//
// if (BeginPlot(...)) {
//     PlotLine(...);
//     ...
//     EndPlot();
// }
//
// Important notes:
//
// - #title_id must be unique to the current ImGui ID scope. If you need to avoid ID
//   collisions or don't want to display a title in the plot, use double hashes
//   (e.g. "MyPlot##HiddenIdText" or "##NoTitle").
// - #size is the **frame** size of the plot widget, not the plot area. The default
//   size of plots (i.e. when ImVec2(0,0)) can be modified in your ImPlotStyle.
IMPLOT_API bool BeginPlot(const char* title_id, const ImVec2& size=ImVec2(-1,0), ImPlotFlags flags=0);

// Only call EndPlot() if BeginPlot() returns true! Typically called at the end
// of an if statement conditioned on BeginPlot(). See example above.
IMPLOT_API void EndPlot();

//-----------------------------------------------------------------------------
// [SECTION] Begin/End Subplots
//-----------------------------------------------------------------------------

// Starts a subdivided plotting context. If the function returns true,
// EndSubplots() MUST be called! Call BeginPlot/EndPlot AT MOST [rows*cols]
// times in  between the begining and end of the subplot context. Plots are
// added in row major order.
//
// Example:
//
// if (BeginSubplots("My Subplot",2,3,ImVec2(800,400)) {
//     for (int i = 0; i < 6; ++i) {
//         if (BeginPlot(...)) {
//             ImPlot::PlotLine(...);
//             ...
//             EndPlot();
//         }
//     }
//     EndSubplots();
// }
//
// Produces:
//
// [0] | [1] | [2]
// ----|-----|----
// [3] | [4] | [5]
//
// Important notes:
//
// - #title_id must be unique to the current ImGui ID scope. If you need to avoid ID
//   collisions or don't want to display a title in the plot, use double hashes
//   (e.g. "MySubplot##HiddenIdText" or "##NoTitle").
// - #rows and #cols must be greater than 0.
// - #size is the size of the entire grid of subplots, not the individual plots
// - #row_ratios and #col_ratios must have AT LEAST #rows and #cols elements,
//   respectively. These are the sizes of the rows and columns expressed in ratios.
//   If the user adjusts the dimensions, the arrays are updated with new ratios.
//
// Important notes regarding BeginPlot from inside of BeginSubplots:
//
// - The #title_id parameter of _BeginPlot_ (see above) does NOT have to be
//   unique when called inside of a subplot context. Subplot IDs are hashed
//   for your convenience so you don't have call PushID or generate unique title
//   strings. Simply pass an empty string to BeginPlot unless you want to title
//   each subplot.
// - The #size parameter of _BeginPlot_ (see above) is ignored when inside of a
//   subplot context. The actual size of the subplot will be based on the
//   #size value you pass to _BeginSubplots_ and #row/#col_ratios if provided.

IMPLOT_API bool BeginSubplots(const char* title_id,
                             int rows,
                             int cols,
                             const ImVec2& size,
                             ImPlotSubplotFlags flags = 0,
                             float* row_ratios        = nullptr,
                             float* col_ratios        = nullptr);

// Only call EndSubplots() if BeginSubplots() returns true! Typically called at the end
// of an if statement conditioned on BeginSublots(). See example above.
IMPLOT_API void EndSubplots();

//-----------------------------------------------------------------------------
// [SECTION] Setup
//-----------------------------------------------------------------------------

// The following API allows you to setup and customize various aspects of the
// current plot. The functions should be called immediately after BeginPlot
// and before any other API calls. Typical usage is as follows:

// if (BeginPlot(...)) {                     1) begin a new plot
//     SetupAxis(ImAxis_X1, "My X-Axis");    2) make Setup calls
//     SetupAxis(ImAxis_Y1, "My Y-Axis");
//     SetupLegend(ImPlotLocation_North);
//     ...
//     SetupFinish();                        3) [optional] explicitly finish setup
//     PlotLine(...);                        4) plot items
//     ...
//     EndPlot();                            5) end the plot
// }
//
// Important notes:
//
// - Always call Setup code at the top of your BeginPlot conditional statement.
// - Setup is locked once you start plotting or explicitly call SetupFinish.
//   Do NOT call Setup code after you begin plotting or after you make
//   any non-Setup API calls (e.g. utils like PlotToPixels also lock Setup)
// - Calling SetupFinish is OPTIONAL, but probably good practice. If you do not
//   call it yourself, then the first subsequent plotting or utility function will
//   call it for you.

// Enables an axis or sets the label and/or flags for an existing axis. Leave #label = nullptr for no label.
IMPLOT_API void SetupAxis(ImAxis axis, const char* label=nullptr, ImPlotAxisFlags flags=0);
// Sets an axis range limits. If ImPlotCond_Always is used, the axes limits will be locked. Inversion with v_min > v_max is not supported; use SetupAxisLimits instead.
IMPLOT_API void SetupAxisLimits(ImAxis axis, double v_min, double v_max, ImPlotCond cond = ImPlotCond_Once);
// Links an axis range limits to external values. Set to nullptr for no linkage. The pointer data must remain valid until EndPlot.
IMPLOT_API void SetupAxisLinks(ImAxis axis, double* link_min, double* link_max);
// Sets the format of numeric axis labels via formater specifier (default="%g"). Formated values will be double (i.e. use %f).
IMPLOT_API void SetupAxisFormat(ImAxis axis, const char* fmt);
// Sets the format of numeric axis labels via formatter callback. Given #value, write a label into #buff. Optionally pass user data.
IMPLOT_API void SetupAxisFormat(ImAxis axis, ImPlotFormatter formatter, void* data=nullptr);
// Sets an axis' ticks and optionally the labels. To keep the default ticks, set #keep_default=true.
IMPLOT_API void SetupAxisTicks(ImAxis axis, const double* values, int n_ticks, const char* const labels[]=nullptr, bool keep_default=false);
// Sets an axis' ticks and optionally the labels for the next plot. To keep the default ticks, set #keep_default=true.
IMPLOT_API void SetupAxisTicks(ImAxis axis, double v_min, double v_max, int n_ticks, const char* const labels[]=nullptr, bool keep_default=false);
// Sets an axis' scale using built-in options.
IMPLOT_API void SetupAxisScale(ImAxis axis, ImPlotScale scale);
// Sets an axis' scale using user supplied forward and inverse transfroms.
IMPLOT_API void SetupAxisScale(ImAxis axis, ImPlotTransform forward, ImPlotTransform inverse, void* data=nullptr);
// Sets an axis' limits constraints.
IMPLOT_API void SetupAxisLimitsConstraints(ImAxis axis, double v_min, double v_max);
// Sets an axis' zoom constraints.
IMPLOT_API void SetupAxisZoomConstraints(ImAxis axis, double z_min, double z_max);

// Sets the label and/or flags for primary X and Y axes (shorthand for two calls to SetupAxis).
IMPLOT_API void SetupAxes(const char* x_label, const char* y_label, ImPlotAxisFlags x_flags=0, ImPlotAxisFlags y_flags=0);
// Sets the primary X and Y axes range limits. If ImPlotCond_Always is used, the axes limits will be locked (shorthand for two calls to SetupAxisLimits).
IMPLOT_API void SetupAxesLimits(double x_min, double x_max, double y_min, double y_max, ImPlotCond cond = ImPlotCond_Once);

// Sets up the plot legend. This can also be called immediately after BeginSubplots when using ImPlotSubplotFlags_ShareItems.
IMPLOT_API void SetupLegend(ImPlotLocation location, ImPlotLegendFlags flags=0);
// Set the location of the current plot's mouse position text (default = South|East).
IMPLOT_API void SetupMouseText(ImPlotLocation location, ImPlotMouseTextFlags flags=0);

// Explicitly finalize plot setup. Once you call this, you cannot make anymore Setup calls for the current plot!
// Note that calling this function is OPTIONAL; it will be called by the first subsequent setup-locking API call.
IMPLOT_API void SetupFinish();

//-----------------------------------------------------------------------------
// [SECTION] SetNext
//-----------------------------------------------------------------------------

// Though you should default to the `Setup` API above, there are some scenarios
// where (re)configuring a plot or axis before `BeginPlot` is needed (e.g. if
// using a preceding button or slider widget to change the plot limits). In
// this case, you can use the `SetNext` API below. While this is not as feature
// rich as the Setup API, most common needs are provided. These functions can be
// called anwhere except for inside of `Begin/EndPlot`. For example:

// if (ImGui::Button("Center Plot"))
//     ImPlot::SetNextPlotLimits(-1,1,-1,1);
// if (ImPlot::BeginPlot(...)) {
//     ...
//     ImPlot::EndPlot();
// }
//
// Important notes:
//
// - You must still enable non-default axes with SetupAxis for these functions
//   to work properly.

// Sets an upcoming axis range limits. If ImPlotCond_Always is used, the axes limits will be locked.
IMPLOT_API void SetNextAxisLimits(ImAxis axis, double v_min, double v_max, ImPlotCond cond = ImPlotCond_Once);
// Links an upcoming axis range limits to external values. Set to nullptr for no linkage. The pointer data must remain valid until EndPlot!
IMPLOT_API void SetNextAxisLinks(ImAxis axis, double* link_min, double* link_max);
// Set an upcoming axis to auto fit to its data.
IMPLOT_API void SetNextAxisToFit(ImAxis axis);

// Sets the upcoming primary X and Y axes range limits. If ImPlotCond_Always is used, the axes limits will be locked (shorthand for two calls to SetupAxisLimits).
IMPLOT_API void SetNextAxesLimits(double x_min, double x_max, double y_min, double y_max, ImPlotCond cond = ImPlotCond_Once);
// Sets all upcoming axes to auto fit to their data.
IMPLOT_API void SetNextAxesToFit();

//-----------------------------------------------------------------------------
// [SECTION] Plot Items
//-----------------------------------------------------------------------------

// The main plotting API is provied below. Call these functions between
// Begin/EndPlot and after any Setup API calls. Each plots data on the current
// x and y axes, which can be changed with `SetAxis/Axes`.
//
// The templated functions are explicitly instantiated in implot_items.cpp.
// They are not intended to be used generically with custom types. You will get
// a linker error if you try! All functions support the following scalar types:
//
// float, double, ImS8, ImU8, ImS16, ImU16, ImS32, ImU32, ImS64, ImU64
//
//
// If you need to plot custom or non-homogenous data you have a few options:
//
// 1. If your data is a simple struct/class (e.g. Vector2f), you can use striding.
//    This is the most performant option if applicable.
//
//    struct Vector2f { float X, Y; };
//    ...
//    Vector2f data[42];
//    ImPlot::PlotLine("line", &data[0].x, &data[0].y, 42, 0, 0, sizeof(Vector2f));
//
// 2. Write a custom getter C function or C++ lambda and pass it and optionally your data to
//    an ImPlot function post-fixed with a G (e.g. PlotScatterG). This has a slight performance
//    cost, but probably not enough to worry about unless your data is very large. Examples:
//
//    ImPlotPoint MyDataGetter(void* data, int idx) {
//        MyData* my_data = (MyData*)data;
//        ImPlotPoint p;
//        p.x = my_data->GetTime(idx);
//        p.y = my_data->GetValue(idx);
//        return p
//    }
//    ...
//    auto my_lambda = [](int idx, void*) {
//        double t = idx / 999.0;
//        return ImPlotPoint(t, 0.5+0.5*std::sin(2*PI*10*t));
//    };
//    ...
//    if (ImPlot::BeginPlot("MyPlot")) {
//        MyData my_data;
//        ImPlot::PlotScatterG("scatter", MyDataGetter, &my_data, my_data.Size());
//        ImPlot::PlotLineG("line", my_lambda, nullptr, 1000);
//        ImPlot::EndPlot();
//    }
//
// NB: All types are converted to double before plotting. You may lose information
// if you try plotting extremely large 64-bit integral types. Proceed with caution!

// Plots a standard 2D line plot.
IMPLOT_TMP void PlotLine(const char* label_id, const T* values, int count, double xscale=1, double xstart=0, ImPlotLineFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotLine(const char* label_id, const T* xs, const T* ys, int count, ImPlotLineFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_API void PlotLineG(const char* label_id, ImPlotGetter getter, void* data, int count, ImPlotLineFlags flags=0);

// Plots a standard 2D scatter plot. Default marker is ImPlotMarker_Circle.
IMPLOT_TMP void PlotScatter(const char* label_id, const T* values, int count, double xscale=1, double xstart=0, ImPlotScatterFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotScatter(const char* label_id, const T* xs, const T* ys, int count, ImPlotScatterFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_API void PlotScatterG(const char* label_id, ImPlotGetter getter, void* data, int count, ImPlotScatterFlags flags=0);

// Plots a a stairstep graph. The y value is continued constantly to the right from every x position, i.e. the interval [x[i], x[i+1]) has the value y[i]
IMPLOT_TMP void PlotStairs(const char* label_id, const T* values, int count, double xscale=1, double xstart=0, ImPlotStairsFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotStairs(const char* label_id, const T* xs, const T* ys, int count, ImPlotStairsFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_API void PlotStairsG(const char* label_id, ImPlotGetter getter, void* data, int count, ImPlotStairsFlags flags=0);

// Plots a shaded (filled) region between two lines, or a line and a horizontal reference. Set yref to +/-INFINITY for infinite fill extents.
IMPLOT_TMP void PlotShaded(const char* label_id, const T* values, int count, double yref=0, double xscale=1, double xstart=0, ImPlotShadedFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotShaded(const char* label_id, const T* xs, const T* ys, int count, double yref=0, ImPlotShadedFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotShaded(const char* label_id, const T* xs, const T* ys1, const T* ys2, int count, ImPlotShadedFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_API void PlotShadedG(const char* label_id, ImPlotGetter getter1, void* data1, ImPlotGetter getter2, void* data2, int count, ImPlotShadedFlags flags=0);

// Plots a bar graph. Vertical by default. #bar_size and #shift are in plot units.
IMPLOT_TMP void PlotBars(const char* label_id, const T* values, int count, double bar_size=0.67, double shift=0, ImPlotBarsFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotBars(const char* label_id, const T* xs, const T* ys, int count, double bar_size, ImPlotBarsFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_API void PlotBarsG(const char* label_id, ImPlotGetter getter, void* data, int count, double bar_size, ImPlotBarsFlags flags=0);

// Plots a group of bars. #values is a row-major matrix with #item_count rows and #group_count cols. #label_ids should have #item_count elements.
IMPLOT_TMP void PlotBarGroups(const char* const label_ids[], const T* values, int item_count, int group_count, double group_size=0.67, double shift=0, ImPlotBarGroupsFlags flags=0);

// Plots vertical error bar. The label_id should be the same as the label_id of the associated line or bar plot.
IMPLOT_TMP void PlotErrorBars(const char* label_id, const T* xs, const T* ys, const T* err, int count, ImPlotErrorBarsFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotErrorBars(const char* label_id, const T* xs, const T* ys, const T* neg, const T* pos, int count, ImPlotErrorBarsFlags flags=0, int offset=0, int stride=sizeof(T));

// Plots stems. Vertical by default.
IMPLOT_TMP void PlotStems(const char* label_id, const T* values, int count, double ref=0, double scale=1, double start=0, ImPlotStemsFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_TMP void PlotStems(const char* label_id, const T* xs, const T* ys, int count, double ref=0, ImPlotStemsFlags flags=0, int offset=0, int stride=sizeof(T));

// Plots infinite vertical or horizontal lines (e.g. for references or asymptotes).
IMPLOT_TMP void PlotInfLines(const char* label_id, const T* values, int count, ImPlotInfLinesFlags flags=0, int offset=0, int stride=sizeof(T));

// Plots a pie chart. Center and radius are in plot units. #label_fmt can be set to nullptr for no labels.
IMPLOT_TMP void PlotPieChart(const char* const label_ids[], const T* values, int count, double x, double y, double radius, ImPlotFormatter fmt, void* fmt_data=nullptr, double angle0=90, ImPlotPieChartFlags flags=0);
IMPLOT_TMP void PlotPieChart(const char* const label_ids[], const T* values, int count, double x, double y, double radius, const char* label_fmt="%.1f", double angle0=90, ImPlotPieChartFlags flags=0);

// Plots a 2D heatmap chart. Values are expected to be in row-major order by default. Leave #scale_min and scale_max both at 0 for automatic color scaling, or set them to a predefined range. #label_fmt can be set to nullptr for no labels.
IMPLOT_TMP void PlotHeatmap(const char* label_id, const T* values, int rows, int cols, double scale_min=0, double scale_max=0, const char* label_fmt="%.1f", const ImPlotPoint& bounds_min=ImPlotPoint(0,0), const ImPlotPoint& bounds_max=ImPlotPoint(1,1), ImPlotHeatmapFlags flags=0);

// Plots a horizontal histogram. #bins can be a positive integer or an ImPlotBin_ method. If #range is left unspecified, the min/max of #values will be used as the range.
// Otherwise, outlier values outside of the range are not binned. The largest bin count or density is returned.
IMPLOT_TMP double PlotHistogram(const char* label_id, const T* values, int count, int bins=ImPlotBin_Sturges, double bar_scale=1.0, ImPlotRange range=ImPlotRange(), ImPlotHistogramFlags flags=0);

// Plots two dimensional, bivariate histogram as a heatmap. #x_bins and #y_bins can be a positive integer or an ImPlotBin. If #range is left unspecified, the min/max of
// #xs an #ys will be used as the ranges. Otherwise, outlier values outside of range are not binned. The largest bin count or density is returned.
IMPLOT_TMP double PlotHistogram2D(const char* label_id, const T* xs, const T* ys, int count, int x_bins=ImPlotBin_Sturges, int y_bins=ImPlotBin_Sturges, ImPlotRect range=ImPlotRect(), ImPlotHistogramFlags flags=0);

// Plots digital data. Digital plots do not respond to y drag or zoom, and are always referenced to the bottom of the plot.
IMPLOT_TMP void PlotDigital(const char* label_id, const T* xs, const T* ys, int count, ImPlotDigitalFlags flags=0, int offset=0, int stride=sizeof(T));
IMPLOT_API void PlotDigitalG(const char* label_id, ImPlotGetter getter, void* data, int count, ImPlotDigitalFlags flags=0);

// Plots an axis-aligned image. #bounds_min/bounds_max are in plot coordinates (y-up) and #uv0/uv1 are in texture coordinates (y-down).
IMPLOT_API void PlotImage(const char* label_id, ImTextureID user_texture_id, const ImPlotPoint& bounds_min, const ImPlotPoint& bounds_max, const ImVec2& uv0=ImVec2(0,0), const ImVec2& uv1=ImVec2(1,1), const ImVec4& tint_col=ImVec4(1,1,1,1), ImPlotImageFlags flags=0);

// Plots a centered text label at point x,y with an optional pixel offset. Text color can be changed with ImPlot::PushStyleColor(ImPlotCol_InlayText, ...).
IMPLOT_API void PlotText(const char* text, double x, double y, const ImVec2& pix_offset=ImVec2(0,0), ImPlotTextFlags flags=0);

// Plots a dummy item (i.e. adds a legend entry colored by ImPlotCol_Line)
IMPLOT_API void PlotDummy(const char* label_id, ImPlotDummyFlags flags=0);

//-----------------------------------------------------------------------------
// [SECTION] Plot Tools
//-----------------------------------------------------------------------------

// The following can be used to render interactive elements and/or annotations.
// Like the item plotting functions above, they apply to the current x and y
// axes, which can be changed with `SetAxis/SetAxes`. These functions return true
// when user interaction causes the provided coordinates to change. Additional
// user interactions can be retrieved through the optional output parameters.

// Shows a draggable point at x,y. #col defaults to ImGuiCol_Text.
IMPLOT_API bool DragPoint(int id, double* x, double* y, const ImVec4& col, float size = 4, ImPlotDragToolFlags flags = 0, bool* out_clicked = nullptr, bool* out_hovered = nullptr, bool* held = nullptr);
// Shows a draggable vertical guide line at an x-value. #col defaults to ImGuiCol_Text.
IMPLOT_API bool DragLineX(int id, double* x, const ImVec4& col, float thickness = 1, ImPlotDragToolFlags flags = 0, bool* out_clicked = nullptr, bool* out_hovered = nullptr, bool* held = nullptr);
// Shows a draggable horizontal guide line at a y-value. #col defaults to ImGuiCol_Text.
IMPLOT_API bool DragLineY(int id, double* y, const ImVec4& col, float thickness = 1, ImPlotDragToolFlags flags = 0, bool* out_clicked = nullptr, bool* out_hovered = nullptr, bool* held = nullptr);
// Shows a draggable and resizeable rectangle.
IMPLOT_API bool DragRect(int id, double* x1, double* y1, double* x2, double* y2, const ImVec4& col, ImPlotDragToolFlags flags = 0, bool* out_clicked = nullptr, bool* out_hovered = nullptr, bool* held = nullptr);

// Shows an annotation callout at a chosen point. Clamping keeps annotations in the plot area. Annotations are always rendered on top.
IMPLOT_API void Annotation(double x, double y, const ImVec4& col, const ImVec2& pix_offset, bool clamp, bool round = false);
IMPLOT_API void Annotation(double x, double y, const ImVec4& col, const ImVec2& pix_offset, bool clamp, const char* fmt, ...)           IM_FMTARGS(6);
IMPLOT_API void AnnotationV(double x, double y, const ImVec4& col, const ImVec2& pix_offset, bool clamp, const char* fmt, va_list args) IM_FMTLIST(6);

// Shows a x-axis tag at the specified coordinate value.
IMPLOT_API void TagX(double x, const ImVec4& col, bool round = false);
IMPLOT_API void TagX(double x, const ImVec4& col, const char* fmt, ...)           IM_FMTARGS(3);
IMPLOT_API void TagXV(double x, const ImVec4& col, const char* fmt, va_list args) IM_FMTLIST(3);

// Shows a y-axis tag at the specified coordinate value.
IMPLOT_API void TagY(double y, const ImVec4& col, bool round = false);
IMPLOT_API void TagY(double y, const ImVec4& col, const char* fmt, ...)           IM_FMTARGS(3);
IMPLOT_API void TagYV(double y, const ImVec4& col, const char* fmt, va_list args) IM_FMTLIST(3);

//-----------------------------------------------------------------------------
// [SECTION] Plot Utils
//-----------------------------------------------------------------------------

// Select which axis/axes will be used for subsequent plot elements.
IMPLOT_API void SetAxis(ImAxis axis);
IMPLOT_API void SetAxes(ImAxis x_axis, ImAxis y_axis);

// Convert pixels to a position in the current plot's coordinate system. Passing IMPLOT_AUTO uses the current axes.
IMPLOT_API ImPlotPoint PixelsToPlot(const ImVec2& pix, ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);
IMPLOT_API ImPlotPoint PixelsToPlot(float x, float y, ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);

// Convert a position in the current plot's coordinate system to pixels. Passing IMPLOT_AUTO uses the current axes.
IMPLOT_API ImVec2 PlotToPixels(const ImPlotPoint& plt, ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);
IMPLOT_API ImVec2 PlotToPixels(double x, double y, ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);

// Get the current Plot position (top-left) in pixels.
IMPLOT_API ImVec2 GetPlotPos();
// Get the curent Plot size in pixels.
IMPLOT_API ImVec2 GetPlotSize();

// Returns the mouse position in x,y coordinates of the current plot. Passing IMPLOT_AUTO uses the current axes.
IMPLOT_API ImPlotPoint GetPlotMousePos(ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);
// Returns the current plot axis range.
IMPLOT_API ImPlotRect GetPlotLimits(ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);

// Returns true if the plot area in the current plot is hovered.
IMPLOT_API bool IsPlotHovered();
// Returns true if the axis label area in the current plot is hovered.
IMPLOT_API bool IsAxisHovered(ImAxis axis);
// Returns true if the bounding frame of a subplot is hovered.
IMPLOT_API bool IsSubplotsHovered();

// Returns true if the current plot is being box selected.
IMPLOT_API bool IsPlotSelected();
// Returns the current plot box selection bounds. Passing IMPLOT_AUTO uses the current axes.
IMPLOT_API ImPlotRect GetPlotSelection(ImAxis x_axis = IMPLOT_AUTO, ImAxis y_axis = IMPLOT_AUTO);
// Cancels a the current plot box selection.
IMPLOT_API void CancelPlotSelection();

// Hides or shows the next plot item (i.e. as if it were toggled from the legend).
// Use ImPlotCond_Always if you need to forcefully set this every frame.
IMPLOT_API void HideNextItem(bool hidden = true, ImPlotCond cond = ImPlotCond_Once);

// Use the following around calls to Begin/EndPlot to align l/r/t/b padding.
// Consider using Begin/EndSubplots first. They are more feature rich and
// accomplish the same behaviour by default. The functions below offer lower
// level control of plot alignment.

// Align axis padding over multiple plots in a single row or column. #group_id must
// be unique. If this function returns true, EndAlignedPlots() must be called.
IMPLOT_API bool BeginAlignedPlots(const char* group_id, bool vertical = true);
// Only call EndAlignedPlots() if BeginAlignedPlots() returns true!
IMPLOT_API void EndAlignedPlots();

//-----------------------------------------------------------------------------
// [SECTION] Legend Utils
//-----------------------------------------------------------------------------

// Begin a popup for a legend entry.
IMPLOT_API bool BeginLegendPopup(const char* label_id, ImGuiMouseButton mouse_button=1);
// End a popup for a legend entry.
IMPLOT_API void EndLegendPopup();
// Returns true if a plot item legend entry is hovered.
IMPLOT_API bool IsLegendEntryHovered(const char* label_id);

//-----------------------------------------------------------------------------
// [SECTION] Drag and Drop
//-----------------------------------------------------------------------------

// Turns the current plot's plotting area into a drag and drop target. Don't forget to call EndDragDropTarget!
IMPLOT_API bool BeginDragDropTargetPlot();
// Turns the current plot's X-axis into a drag and drop target. Don't forget to call EndDragDropTarget!
IMPLOT_API bool BeginDragDropTargetAxis(ImAxis axis);
// Turns the current plot's legend into a drag and drop target. Don't forget to call EndDragDropTarget!
IMPLOT_API bool BeginDragDropTargetLegend();
// Ends a drag and drop target (currently just an alias for ImGui::EndDragDropTarget).
IMPLOT_API void EndDragDropTarget();

// NB: By default, plot and axes drag and drop *sources* require holding the Ctrl modifier to initiate the drag.
// You can change the modifier if desired. If ImGuiMod_None is provided, the axes will be locked from panning.

// Turns the current plot's plotting area into a drag and drop source. You must hold Ctrl. Don't forget to call EndDragDropSource!
IMPLOT_API bool BeginDragDropSourcePlot(ImGuiDragDropFlags flags=0);
// Turns the current plot's X-axis into a drag and drop source. You must hold Ctrl. Don't forget to call EndDragDropSource!
IMPLOT_API bool BeginDragDropSourceAxis(ImAxis axis, ImGuiDragDropFlags flags=0);
// Turns an item in the current plot's legend into drag and drop source. Don't forget to call EndDragDropSource!
IMPLOT_API bool BeginDragDropSourceItem(const char* label_id, ImGuiDragDropFlags flags=0);
// Ends a drag and drop source (currently just an alias for ImGui::EndDragDropSource).
IMPLOT_API void EndDragDropSource();

//-----------------------------------------------------------------------------
// [SECTION] Styling
//-----------------------------------------------------------------------------

// Styling colors in ImPlot works similarly to styling colors in ImGui, but
// with one important difference. Like ImGui, all style colors are stored in an
// indexable array in ImPlotStyle. You can permanently modify these values through
// GetStyle().Colors, or temporarily modify them with Push/Pop functions below.
// However, by default all style colors in ImPlot default to a special color
// IMPLOT_AUTO_COL. The behavior of this color depends upon the style color to
// which it as applied:
//
//     1) For style colors associated with plot items (e.g. ImPlotCol_Line),
//        IMPLOT_AUTO_COL tells ImPlot to color the item with the next unused
//        color in the current colormap. Thus, every item will have a different
//        color up to the number of colors in the colormap, at which point the
//        colormap will roll over. For most use cases, you should not need to
//        set these style colors to anything but IMPLOT_COL_AUTO; you are
//        probably better off changing the current colormap. However, if you
//        need to explicitly color a particular item you may either Push/Pop
//        the style color around the item in question, or use the SetNextXXXStyle
//        API below. If you permanently set one of these style colors to a specific
//        color, or forget to call Pop, then all subsequent items will be styled
//        with the color you set.
//
//     2) For style colors associated with plot styling (e.g. ImPlotCol_PlotBg),
//        IMPLOT_AUTO_COL tells ImPlot to set that color from color data in your
//        **ImGuiStyle**. The ImGuiCol_ that these style colors default to are
//        detailed above, and in general have been mapped to produce plots visually
//        consistent with your current ImGui style. Of course, you are free to
//        manually set these colors to whatever you like, and further can Push/Pop
//        them around individual plots for plot-specific styling (e.g. coloring axes).

// Provides access to plot style structure for permanant modifications to colors, sizes, etc.
IMPLOT_API ImPlotStyle& GetStyle();

// Style plot colors for current ImGui style (default).
IMPLOT_API void StyleColorsAuto(ImPlotStyle* dst = nullptr);
// Style plot colors for ImGui "Classic".
IMPLOT_API void StyleColorsClassic(ImPlotStyle* dst = nullptr);
// Style plot colors for ImGui "Dark".
IMPLOT_API void StyleColorsDark(ImPlotStyle* dst = nullptr);
// Style plot colors for ImGui "Light".
IMPLOT_API void StyleColorsLight(ImPlotStyle* dst = nullptr);

// Use PushStyleX to temporarily modify your ImPlotStyle. The modification
// will last until the matching call to PopStyleX. You MUST call a pop for
// every push, otherwise you will leak memory! This behaves just like ImGui.

// Temporarily modify a style color. Don't forget to call PopStyleColor!
IMPLOT_API void PushStyleColor(ImPlotCol idx, ImU32 col);
IMPLOT_API void PushStyleColor(ImPlotCol idx, const ImVec4& col);
// Undo temporary style color modification(s). Undo multiple pushes at once by increasing count.
IMPLOT_API void PopStyleColor(int count = 1);

// Temporarily modify a style variable of float type. Don't forget to call PopStyleVar!
IMPLOT_API void PushStyleVar(ImPlotStyleVar idx, float val);
// Temporarily modify a style variable of int type. Don't forget to call PopStyleVar!
IMPLOT_API void PushStyleVar(ImPlotStyleVar idx, int val);
// Temporarily modify a style variable of ImVec2 type. Don't forget to call PopStyleVar!
IMPLOT_API void PushStyleVar(ImPlotStyleVar idx, const ImVec2& val);
// Undo temporary style variable modification(s). Undo multiple pushes at once by increasing count.
IMPLOT_API void PopStyleVar(int count = 1);

// The following can be used to modify the style of the next plot item ONLY. They do
// NOT require calls to PopStyleX. Leave style attributes you don't want modified to
// IMPLOT_AUTO or IMPLOT_AUTO_COL. Automatic styles will be deduced from the current
// values in your ImPlotStyle or from Colormap data.

// Set the line color and weight for the next item only.
IMPLOT_API void SetNextLineStyle(const ImVec4& col = IMPLOT_AUTO_COL, float weight = IMPLOT_AUTO);
// Set the fill color for the next item only.
IMPLOT_API void SetNextFillStyle(const ImVec4& col = IMPLOT_AUTO_COL, float alpha_mod = IMPLOT_AUTO);
// Set the marker style for the next item only.
IMPLOT_API void SetNextMarkerStyle(ImPlotMarker marker = IMPLOT_AUTO, float size = IMPLOT_AUTO, const ImVec4& fill = IMPLOT_AUTO_COL, float weight = IMPLOT_AUTO, const ImVec4& outline = IMPLOT_AUTO_COL);
// Set the error bar style for the next item only.
IMPLOT_API void SetNextErrorBarStyle(const ImVec4& col = IMPLOT_AUTO_COL, float size = IMPLOT_AUTO, float weight = IMPLOT_AUTO);

// Gets the last item primary color (i.e. its legend icon color)
IMPLOT_API ImVec4 GetLastItemColor();

// Returns the null terminated string name for an ImPlotCol.
IMPLOT_API const char* GetStyleColorName(ImPlotCol idx);
// Returns the null terminated string name for an ImPlotMarker.
IMPLOT_API const char* GetMarkerName(ImPlotMarker idx);

//-----------------------------------------------------------------------------
// [SECTION] Colormaps
//-----------------------------------------------------------------------------

// Item styling is based on colormaps when the relevant ImPlotCol_XXX is set to
// IMPLOT_AUTO_COL (default). Several built-in colormaps are available. You can
// add and then push/pop your own colormaps as well. To permanently set a colormap,
// modify the Colormap index member of your ImPlotStyle.

// Colormap data will be ignored and a custom color will be used if you have done one of the following:
//     1) Modified an item style color in your ImPlotStyle to anything other than IMPLOT_AUTO_COL.
//     2) Pushed an item style color using PushStyleColor().
//     3) Set the next item style with a SetNextXXXStyle function.

// Add a new colormap. The color data will be copied. The colormap can be used by pushing either the returned index or the
// string name with PushColormap. The colormap name must be unique and the size must be greater than 1. You will receive
// an assert otherwise! By default colormaps are considered to be qualitative (i.e. discrete). If you want to create a
// continuous colormap, set #qual=false. This will treat the colors you provide as keys, and ImPlot will build a linearly
// interpolated lookup table. The memory footprint of this table will be exactly ((size-1)*255+1)*4 bytes.
IMPLOT_API ImPlotColormap AddColormap(const char* name, const ImVec4* cols, int size, bool qual=true);
IMPLOT_API ImPlotColormap AddColormap(const char* name, const ImU32*  cols, int size, bool qual=true);

// Returns the number of available colormaps (i.e. the built-in + user-added count).
IMPLOT_API int GetColormapCount();
// Returns a null terminated string name for a colormap given an index. Returns nullptr if index is invalid.
IMPLOT_API const char* GetColormapName(ImPlotColormap cmap);
// Returns an index number for a colormap given a valid string name. Returns -1 if name is invalid.
IMPLOT_API ImPlotColormap GetColormapIndex(const char* name);

// Temporarily switch to one of the built-in (i.e. ImPlotColormap_XXX) or user-added colormaps (i.e. a return value of AddColormap). Don't forget to call PopColormap!
IMPLOT_API void PushColormap(ImPlotColormap cmap);
// Push a colormap by string name. Use built-in names such as "Default", "Deep", "Jet", etc. or a string you provided to AddColormap. Don't forget to call PopColormap!
IMPLOT_API void PushColormap(const char* name);
// Undo temporary colormap modification(s). Undo multiple pushes at once by increasing count.
IMPLOT_API void PopColormap(int count = 1);

// Returns the next color from the current colormap and advances the colormap for the current plot.
// Can also be used with no return value to skip colors if desired. You need to call this between Begin/EndPlot!
IMPLOT_API ImVec4 NextColormapColor();

// Colormap utils. If cmap = IMPLOT_AUTO (default), the current colormap is assumed.
// Pass an explicit colormap index (built-in or user-added) to specify otherwise.

// Returns the size of a colormap.
IMPLOT_API int GetColormapSize(ImPlotColormap cmap = IMPLOT_AUTO);
// Returns a color from a colormap given an index >= 0 (modulo will be performed).
IMPLOT_API ImVec4 GetColormapColor(int idx, ImPlotColormap cmap = IMPLOT_AUTO);
// Sample a color from the current colormap given t between 0 and 1.
IMPLOT_API ImVec4 SampleColormap(float t, ImPlotColormap cmap = IMPLOT_AUTO);

// Shows a vertical color scale with linear spaced ticks using the specified color map. Use double hashes to hide label (e.g. "##NoLabel"). If scale_min > scale_max, the scale to color mapping will be reversed.
IMPLOT_API void ColormapScale(const char* label, double scale_min, double scale_max, const ImVec2& size = ImVec2(0,0), const char* format = "%g", ImPlotColormapScaleFlags flags = 0, ImPlotColormap cmap = IMPLOT_AUTO);
// Shows a horizontal slider with a colormap gradient background. Optionally returns the color sampled at t in [0 1].
IMPLOT_API bool ColormapSlider(const char* label, float* t, ImVec4* out = nullptr, const char* format = "", ImPlotColormap cmap = IMPLOT_AUTO);
// Shows a button with a colormap gradient brackground.
IMPLOT_API bool ColormapButton(const char* label, const ImVec2& size = ImVec2(0,0), ImPlotColormap cmap = IMPLOT_AUTO);

// When items in a plot sample their color from a colormap, the color is cached and does not change
// unless explicitly overriden. Therefore, if you change the colormap after the item has already been plotted,
// item colors will NOT update. If you need item colors to resample the new colormap, then use this
// function to bust the cached colors. If #plot_title_id is nullptr, then every item in EVERY existing plot
// will be cache busted. Otherwise only the plot specified by #plot_title_id will be busted. For the
// latter, this function must be called in the same ImGui ID scope that the plot is in. You should rarely if ever
// need this function, but it is available for applications that require runtime colormap swaps (e.g. Heatmaps demo).
IMPLOT_API void BustColorCache(const char* plot_title_id = nullptr);

//-----------------------------------------------------------------------------
// [SECTION] Input Mapping
//-----------------------------------------------------------------------------

// Provides access to input mapping structure for permanant modifications to controls for pan, select, etc.
IMPLOT_API ImPlotInputMap& GetInputMap();

// Default input mapping: pan = LMB drag, box select = RMB drag, fit = LMB double click, context menu = RMB click, zoom = scroll.
IMPLOT_API void MapInputDefault(ImPlotInputMap* dst = nullptr);
// Reverse input mapping: pan = RMB drag, box select = LMB drag, fit = LMB double click, context menu = RMB click, zoom = scroll.
IMPLOT_API void MapInputReverse(ImPlotInputMap* dst = nullptr);

//-----------------------------------------------------------------------------
// [SECTION] Miscellaneous
//-----------------------------------------------------------------------------

// Render icons similar to those that appear in legends (nifty for data lists).
IMPLOT_API void ItemIcon(const ImVec4& col);
IMPLOT_API void ItemIcon(ImU32 col);
IMPLOT_API void ColormapIcon(ImPlotColormap cmap);

// Get the plot draw list for custom rendering to the current plot area. Call between Begin/EndPlot.
IMPLOT_API ImDrawList* GetPlotDrawList();
// Push clip rect for rendering to current plot area. The rect can be expanded or contracted by #expand pixels. Call between Begin/EndPlot.
IMPLOT_API void PushPlotClipRect(float expand=0);
// Pop plot clip rect. Call between Begin/EndPlot.
IMPLOT_API void PopPlotClipRect();

// Shows ImPlot style selector dropdown menu.
IMPLOT_API bool ShowStyleSelector(const char* label);
// Shows ImPlot colormap selector dropdown menu.
IMPLOT_API bool ShowColormapSelector(const char* label);
// Shows ImPlot input map selector dropdown menu.
IMPLOT_API bool ShowInputMapSelector(const char* label);
// Shows ImPlot style editor block (not a window).
IMPLOT_API void ShowStyleEditor(ImPlotStyle* ref = nullptr);
// Add basic help/info block for end users (not a window).
IMPLOT_API void ShowUserGuide();
// Shows ImPlot metrics/debug information window.
IMPLOT_API void ShowMetricsWindow(bool* p_popen = nullptr);

//-----------------------------------------------------------------------------
// [SECTION] Demo
//-----------------------------------------------------------------------------

// Shows the ImPlot demo window (add implot_demo.cpp to your sources!)
IMPLOT_API void ShowDemoWindow(bool* p_open = nullptr);

}  // namespace ImPlot

//-----------------------------------------------------------------------------
// [SECTION] Obsolete API
//-----------------------------------------------------------------------------

// The following functions will be removed! Keep your copy of implot up to date!
// Occasionally set '#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS' to stay ahead.
// If you absolutely must use these functions and do not want to receive compiler
// warnings, set '#define IMPLOT_DISABLE_OBSOLETE_WARNINGS'.

#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS

#ifndef IMPLOT_DISABLE_DEPRECATED_WARNINGS
#if __cplusplus > 201402L
#define IMPLOT_DEPRECATED(method) [[deprecated]] method
#elif defined( __GNUC__ ) && !defined( __INTEL_COMPILER ) && ( __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ >= 1 ) )
#define IMPLOT_DEPRECATED(method) method __attribute__( ( deprecated ) )
#elif defined( _MSC_VER )
#define IMPLOT_DEPRECATED(method) __declspec(deprecated) method
#else
#define IMPLOT_DEPRECATED(method) method
#endif
#else
#define IMPLOT_DEPRECATED(method) method
#endif

enum ImPlotFlagsObsolete_ {
    ImPlotFlags_YAxis2 = 1 << 20,
    ImPlotFlags_YAxis3 = 1 << 21,
};

namespace ImPlot {

// OBSOLETED in v0.13 -> PLANNED REMOVAL in v1.0
IMPLOT_DEPRECATED( IMPLOT_API bool BeginPlot(const char* title_id,
                                             const char* x_label,  // = nullptr,
                                             const char* y_label,  // = nullptr,
                                             const ImVec2& size       = ImVec2(-1,0),
                                             ImPlotFlags flags        = ImPlotFlags_None,
                                             ImPlotAxisFlags x_flags  = 0,
                                             ImPlotAxisFlags y_flags  = 0,
                                             ImPlotAxisFlags y2_flags = ImPlotAxisFlags_AuxDefault,
                                             ImPlotAxisFlags y3_flags = ImPlotAxisFlags_AuxDefault,
                                             const char* y2_label     = nullptr,
                                             const char* y3_label     = nullptr) );

} // namespace ImPlot

#endif

// MIT License

// Copyright (c) 2023 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.17

// You may use this file to debug, understand or extend ImPlot features but we
// don't provide any guarantee of forward compatibility!

//-----------------------------------------------------------------------------
// [SECTION] Header Mess
//-----------------------------------------------------------------------------

#pragma once

#include <time.h>
//#include "imgui_internal.h"

#ifndef IMPLOT_VERSION
#error Must include implot.h before implot_internal.h
#endif


// Support for pre-1.84 versions. ImPool's GetSize() -> GetBufSize()
#if (IMGUI_VERSION_NUM < 18303)
#define GetBufSize GetSize
#endif

//-----------------------------------------------------------------------------
// [SECTION] Constants
//-----------------------------------------------------------------------------

// Constants can be changed unless stated otherwise. We may move some of these
// to ImPlotStyleVar_ over time.

// Mimimum allowable timestamp value 01/01/1970 @ 12:00am (UTC) (DO NOT DECREASE THIS)
#define IMPLOT_MIN_TIME  0
// Maximum allowable timestamp value 01/01/3000 @ 12:00am (UTC) (DO NOT INCREASE THIS)
#define IMPLOT_MAX_TIME  32503680000
// Default label format for axis labels
#define IMPLOT_LABEL_FORMAT "%g"
// Max character size for tick labels
#define IMPLOT_LABEL_MAX_SIZE 32

//-----------------------------------------------------------------------------
// [SECTION] Macros
//-----------------------------------------------------------------------------

#define IMPLOT_NUM_X_AXES ImAxis_Y1
#define IMPLOT_NUM_Y_AXES (ImAxis_COUNT - IMPLOT_NUM_X_AXES)

// Split ImU32 color into RGB components [0 255]
#define IM_COL32_SPLIT_RGB(col,r,g,b) \
    ImU32 r = ((col >> IM_COL32_R_SHIFT) & 0xFF); \
    ImU32 g = ((col >> IM_COL32_G_SHIFT) & 0xFF); \
    ImU32 b = ((col >> IM_COL32_B_SHIFT) & 0xFF);

//-----------------------------------------------------------------------------
// [SECTION] Forward Declarations
//-----------------------------------------------------------------------------

struct ImPlotTick;
struct ImPlotAxis;
struct ImPlotAxisColor;
struct ImPlotItem;
struct ImPlotLegend;
struct ImPlotPlot;
struct ImPlotNextPlotData;
struct ImPlotTicker;

//-----------------------------------------------------------------------------
// [SECTION] Context Pointer
//-----------------------------------------------------------------------------

#ifndef GImPlot
extern IMPLOT_API ImPlotContext* GImPlot; // Current implicit context pointer
#endif

//-----------------------------------------------------------------------------
// [SECTION] Generic Helpers
//-----------------------------------------------------------------------------

// Computes the common (base-10) logarithm
static inline float  ImLog10(float x)  { return log10f(x); }
static inline double ImLog10(double x) { return log10(x);  }
static inline float  ImSinh(float x)   { return sinhf(x);  }
static inline double ImSinh(double x)  { return sinh(x);   }
static inline float  ImAsinh(float x)  { return asinhf(x); }
static inline double ImAsinh(double x) { return asinh(x);  }
// Returns true if a flag is set
template <typename TSet, typename TFlag>
static inline bool ImHasFlag(TSet set, TFlag flag) { return (set & flag) == flag; }
// Flips a flag in a flagset
template <typename TSet, typename TFlag>
static inline void ImFlipFlag(TSet& set, TFlag flag) { ImHasFlag(set, flag) ? set &= ~flag : set |= flag; }
// Linearly remaps x from [x0 x1] to [y0 y1].
template <typename T>
static inline T ImRemap(T x, T x0, T x1, T y0, T y1) { return y0 + (x - x0) * (y1 - y0) / (x1 - x0); }
// Linear rempas x from [x0 x1] to [0 1]
template <typename T>
static inline T ImRemap01(T x, T x0, T x1) { return (x - x0) / (x1 - x0); }
// Returns always positive modulo (assumes r != 0)
static inline int ImPosMod(int l, int r) { return (l % r + r) % r; }
// Returns true if val is NAN
static inline bool ImNan(double val) { return isnan(val); }
// Returns true if val is NAN or INFINITY
static inline bool ImNanOrInf(double val) { return !(val >= -DBL_MAX && val <= DBL_MAX) || ImNan(val); }
// Turns NANs to 0s
static inline double ImConstrainNan(double val) { return ImNan(val) ? 0 : val; }
// Turns infinity to floating point maximums
static inline double ImConstrainInf(double val) { return val >= DBL_MAX ?  DBL_MAX : val <= -DBL_MAX ? - DBL_MAX : val; }
// Turns numbers less than or equal to 0 to 0.001 (sort of arbitrary, is there a better way?)
static inline double ImConstrainLog(double val) { return val <= 0 ? 0.001f : val; }
// Turns numbers less than 0 to zero
static inline double ImConstrainTime(double val) { return val < IMPLOT_MIN_TIME ? IMPLOT_MIN_TIME : (val > IMPLOT_MAX_TIME ? IMPLOT_MAX_TIME : val); }
// True if two numbers are approximately equal using units in the last place.
static inline bool ImAlmostEqual(double v1, double v2, int ulp = 2) { return ImAbs(v1-v2) < DBL_EPSILON * ImAbs(v1+v2) * ulp || ImAbs(v1-v2) < DBL_MIN; }
// Finds min value in an unsorted array
template <typename T>
static inline T ImMinArray(const T* values, int count) { T m = values[0]; for (int i = 1; i < count; ++i) { if (values[i] < m) { m = values[i]; } } return m; }
// Finds the max value in an unsorted array
template <typename T>
static inline T ImMaxArray(const T* values, int count) { T m = values[0]; for (int i = 1; i < count; ++i) { if (values[i] > m) { m = values[i]; } } return m; }
// Finds the min and max value in an unsorted array
template <typename T>
static inline void ImMinMaxArray(const T* values, int count, T* min_out, T* max_out) {
    T Min = values[0]; T Max = values[0];
    for (int i = 1; i < count; ++i) {
        if (values[i] < Min) { Min = values[i]; }
        if (values[i] > Max) { Max = values[i]; }
    }
    *min_out = Min; *max_out = Max;
}
// Finds the sim of an array
template <typename T>
static inline T ImSum(const T* values, int count) {
    T sum  = 0;
    for (int i = 0; i < count; ++i)
        sum += values[i];
    return sum;
}
// Finds the mean of an array
template <typename T>
static inline double ImMean(const T* values, int count) {
    double den = 1.0 / count;
    double mu  = 0;
    for (int i = 0; i < count; ++i)
        mu += (double)values[i] * den;
    return mu;
}
// Finds the sample standard deviation of an array
template <typename T>
static inline double ImStdDev(const T* values, int count) {
    double den = 1.0 / (count - 1.0);
    double mu  = ImMean(values, count);
    double x   = 0;
    for (int i = 0; i < count; ++i)
        x += ((double)values[i] - mu) * ((double)values[i] - mu) * den;
    return sqrt(x);
}
// Mix color a and b by factor s in [0 256]
static inline ImU32 ImMixU32(ImU32 a, ImU32 b, ImU32 s) {
#ifdef IMPLOT_MIX64
    const ImU32 af = 256-s;
    const ImU32 bf = s;
    const ImU64 al = (a & 0x00ff00ff) | (((ImU64)(a & 0xff00ff00)) << 24);
    const ImU64 bl = (b & 0x00ff00ff) | (((ImU64)(b & 0xff00ff00)) << 24);
    const ImU64 mix = (al * af + bl * bf);
    return ((mix >> 32) & 0xff00ff00) | ((mix & 0xff00ff00) >> 8);
#else
    const ImU32 af = 256-s;
    const ImU32 bf = s;
    const ImU32 al = (a & 0x00ff00ff);
    const ImU32 ah = (a & 0xff00ff00) >> 8;
    const ImU32 bl = (b & 0x00ff00ff);
    const ImU32 bh = (b & 0xff00ff00) >> 8;
    const ImU32 ml = (al * af + bl * bf);
    const ImU32 mh = (ah * af + bh * bf);
    return (mh & 0xff00ff00) | ((ml & 0xff00ff00) >> 8);
#endif
}

// Lerp across an array of 32-bit collors given t in [0.0 1.0]
static inline ImU32 ImLerpU32(const ImU32* colors, int size, float t) {
    int i1 = (int)((size - 1 ) * t);
    int i2 = i1 + 1;
    if (i2 == size || size == 1)
        return colors[i1];
    float den = 1.0f / (size - 1);
    float t1 = i1 * den;
    float t2 = i2 * den;
    float tr = ImRemap01(t, t1, t2);
    return ImMixU32(colors[i1], colors[i2], (ImU32)(tr*256));
}

// Set alpha channel of 32-bit color from float in range [0.0 1.0]
static inline ImU32 ImAlphaU32(ImU32 col, float alpha) {
    return col & ~((ImU32)((1.0f-alpha)*255)<<IM_COL32_A_SHIFT);
}

// Returns true of two ranges overlap
template <typename T>
static inline bool ImOverlaps(T min_a, T max_a, T min_b, T max_b) {
    return min_a <= max_b && min_b <= max_a;
}

//-----------------------------------------------------------------------------
// [SECTION] ImPlot Enums
//-----------------------------------------------------------------------------

typedef int ImPlotTimeUnit;    // -> enum ImPlotTimeUnit_
typedef int ImPlotDateFmt;     // -> enum ImPlotDateFmt_
typedef int ImPlotTimeFmt;     // -> enum ImPlotTimeFmt_

enum ImPlotTimeUnit_ {
    ImPlotTimeUnit_Us,  // microsecond
    ImPlotTimeUnit_Ms,  // millisecond
    ImPlotTimeUnit_S,   // second
    ImPlotTimeUnit_Min, // minute
    ImPlotTimeUnit_Hr,  // hour
    ImPlotTimeUnit_Day, // day
    ImPlotTimeUnit_Mo,  // month
    ImPlotTimeUnit_Yr,  // year
    ImPlotTimeUnit_COUNT
};

enum ImPlotDateFmt_ {              // default        [ ISO 8601     ]
    ImPlotDateFmt_None = 0,
    ImPlotDateFmt_DayMo,           // 10/3           [ --10-03      ]
    ImPlotDateFmt_DayMoYr,         // 10/3/91        [ 1991-10-03   ]
    ImPlotDateFmt_MoYr,            // Oct 1991       [ 1991-10      ]
    ImPlotDateFmt_Mo,              // Oct            [ --10         ]
    ImPlotDateFmt_Yr               // 1991           [ 1991         ]
};

enum ImPlotTimeFmt_ {              // default        [ 24 Hour Clock ]
    ImPlotTimeFmt_None = 0,
    ImPlotTimeFmt_Us,              // .428 552       [ .428 552     ]
    ImPlotTimeFmt_SUs,             // :29.428 552    [ :29.428 552  ]
    ImPlotTimeFmt_SMs,             // :29.428        [ :29.428      ]
    ImPlotTimeFmt_S,               // :29            [ :29          ]
    ImPlotTimeFmt_MinSMs,          // 21:29.428      [ 21:29.428    ]
    ImPlotTimeFmt_HrMinSMs,        // 7:21:29.428pm  [ 19:21:29.428 ]
    ImPlotTimeFmt_HrMinS,          // 7:21:29pm      [ 19:21:29     ]
    ImPlotTimeFmt_HrMin,           // 7:21pm         [ 19:21        ]
    ImPlotTimeFmt_Hr               // 7pm            [ 19:00        ]
};

//-----------------------------------------------------------------------------
// [SECTION] Callbacks
//-----------------------------------------------------------------------------

typedef void (*ImPlotLocator)(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data);

//-----------------------------------------------------------------------------
// [SECTION] Structs
//-----------------------------------------------------------------------------

// Combined date/time format spec
struct ImPlotDateTimeSpec {
    ImPlotDateTimeSpec() {}
    ImPlotDateTimeSpec(ImPlotDateFmt date_fmt, ImPlotTimeFmt time_fmt, bool use_24_hr_clk = false, bool use_iso_8601 = false) {
        Date           = date_fmt;
        Time           = time_fmt;
        UseISO8601     = use_iso_8601;
        Use24HourClock = use_24_hr_clk;
    }
    ImPlotDateFmt Date;
    ImPlotTimeFmt Time;
    bool UseISO8601;
    bool Use24HourClock;
};

// Two part timestamp struct.
struct ImPlotTime {
    time_t S;  // second part
    int    Us; // microsecond part
    ImPlotTime() { S = 0; Us = 0; }
    ImPlotTime(time_t s, int us = 0) { S  = s + us / 1000000; Us = us % 1000000; }
    void RollOver() { S  = S + Us / 1000000;  Us = Us % 1000000; }
    double ToDouble() const { return (double)S + (double)Us / 1000000.0; }
    static ImPlotTime FromDouble(double t) { return ImPlotTime((time_t)t, (int)(t * 1000000 - floor(t) * 1000000)); }
};

static inline ImPlotTime operator+(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return ImPlotTime(lhs.S + rhs.S, lhs.Us + rhs.Us); }
static inline ImPlotTime operator-(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return ImPlotTime(lhs.S - rhs.S, lhs.Us - rhs.Us); }
static inline bool operator==(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return lhs.S == rhs.S && lhs.Us == rhs.Us; }
static inline bool operator<(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return lhs.S == rhs.S ? lhs.Us < rhs.Us : lhs.S < rhs.S; }
static inline bool operator>(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return rhs < lhs; }
static inline bool operator<=(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return lhs < rhs || lhs == rhs; }
static inline bool operator>=(const ImPlotTime& lhs, const ImPlotTime& rhs)
{ return lhs > rhs || lhs == rhs; }

// Colormap data storage
struct ImPlotColormapData {
    ImVector<ImU32> Keys;
    ImVector<int>   KeyCounts;
    ImVector<int>   KeyOffsets;
    ImVector<ImU32> Tables;
    ImVector<int>   TableSizes;
    ImVector<int>   TableOffsets;
    ImGuiTextBuffer Text;
    ImVector<int>   TextOffsets;
    ImVector<bool>  Quals;
    ImGuiStorage    Map;
    int             Count;

    ImPlotColormapData() { Count = 0; }

    int Append(const char* name, const ImU32* keys, int count, bool qual) {
        if (GetIndex(name) != -1)
            return -1;
        KeyOffsets.push_back(Keys.size());
        KeyCounts.push_back(count);
        Keys.reserve(Keys.size()+count);
        for (int i = 0; i < count; ++i)
            Keys.push_back(keys[i]);
        TextOffsets.push_back(Text.size());
        Text.append(name, name + strlen(name) + 1);
        Quals.push_back(qual);
        ImGuiID id = ImHashStr(name);
        int idx = Count++;
        Map.SetInt(id,idx);
        _AppendTable(idx);
        return idx;
    }

    void _AppendTable(ImPlotColormap cmap) {
        int key_count     = GetKeyCount(cmap);
        const ImU32* keys = GetKeys(cmap);
        int off = Tables.size();
        TableOffsets.push_back(off);
        if (IsQual(cmap)) {
            Tables.reserve(key_count);
            for (int i = 0; i < key_count; ++i)
                Tables.push_back(keys[i]);
            TableSizes.push_back(key_count);
        }
        else {
            int max_size = 255 * (key_count-1) + 1;
            Tables.reserve(off + max_size);
            // ImU32 last = keys[0];
            // Tables.push_back(last);
            // int n = 1;
            for (int i = 0; i < key_count-1; ++i) {
                for (int s = 0; s < 255; ++s) {
                    ImU32 a = keys[i];
                    ImU32 b = keys[i+1];
                    ImU32 c = ImMixU32(a,b,s);
                    // if (c != last) {
                        Tables.push_back(c);
                        // last = c;
                        // n++;
                    // }
                }
            }
            ImU32 c = keys[key_count-1];
            // if (c != last) {
                Tables.push_back(c);
                // n++;
            // }
            // TableSizes.push_back(n);
            TableSizes.push_back(max_size);
        }
    }

    void RebuildTables() {
        Tables.resize(0);
        TableSizes.resize(0);
        TableOffsets.resize(0);
        for (int i = 0; i < Count; ++i)
            _AppendTable(i);
    }

    inline bool           IsQual(ImPlotColormap cmap) const                      { return Quals[cmap];                                                }
    inline const char*    GetName(ImPlotColormap cmap) const                     { return cmap < Count ? Text.Buf.Data + TextOffsets[cmap] : nullptr; }
    inline ImPlotColormap GetIndex(const char* name) const                       { ImGuiID key = ImHashStr(name); return Map.GetInt(key,-1);          }

    inline const ImU32*   GetKeys(ImPlotColormap cmap) const                     { return &Keys[KeyOffsets[cmap]];                                    }
    inline int            GetKeyCount(ImPlotColormap cmap) const                 { return KeyCounts[cmap];                                            }
    inline ImU32          GetKeyColor(ImPlotColormap cmap, int idx) const        { return Keys[KeyOffsets[cmap]+idx];                                 }
    inline void           SetKeyColor(ImPlotColormap cmap, int idx, ImU32 value) { Keys[KeyOffsets[cmap]+idx] = value; RebuildTables();               }

    inline const ImU32*   GetTable(ImPlotColormap cmap) const                    { return &Tables[TableOffsets[cmap]];                                }
    inline int            GetTableSize(ImPlotColormap cmap) const                { return TableSizes[cmap];                                           }
    inline ImU32          GetTableColor(ImPlotColormap cmap, int idx) const      { return Tables[TableOffsets[cmap]+idx];                             }

    inline ImU32 LerpTable(ImPlotColormap cmap, float t) const {
        int off = TableOffsets[cmap];
        int siz = TableSizes[cmap];
        int idx = Quals[cmap] ? ImClamp((int)(siz*t),0,siz-1) : (int)((siz - 1) * t + 0.5f);
        return Tables[off + idx];
    }
};

// ImPlotPoint with positive/negative error values
struct ImPlotPointError {
    double X, Y, Neg, Pos;
    ImPlotPointError(double x, double y, double neg, double pos) {
        X = x; Y = y; Neg = neg; Pos = pos;
    }
};

// Interior plot label/annotation
struct ImPlotAnnotation {
    ImVec2 Pos;
    ImVec2 Offset;
    ImU32  ColorBg;
    ImU32  ColorFg;
    int    TextOffset;
    bool   Clamp;
    ImPlotAnnotation() {
        ColorBg = ColorFg = 0;
        TextOffset = 0;
        Clamp = false;
    }
};

// Collection of plot labels
struct ImPlotAnnotationCollection {

    ImVector<ImPlotAnnotation> Annotations;
    ImGuiTextBuffer            TextBuffer;
    int                        Size;

    ImPlotAnnotationCollection() { Reset(); }

    void AppendV(const ImVec2& pos, const ImVec2& off, ImU32 bg, ImU32 fg, bool clamp, const char* fmt,  va_list args) IM_FMTLIST(7) {
        ImPlotAnnotation an;
        an.Pos = pos; an.Offset = off;
        an.ColorBg = bg; an.ColorFg = fg;
        an.TextOffset = TextBuffer.size();
        an.Clamp = clamp;
        Annotations.push_back(an);
        TextBuffer.appendfv(fmt, args);
        const char nul[] = "";
        TextBuffer.append(nul,nul+1);
        Size++;
    }

    void Append(const ImVec2& pos, const ImVec2& off, ImU32 bg, ImU32 fg, bool clamp, const char* fmt,  ...) IM_FMTARGS(7) {
        va_list args;
        va_start(args, fmt);
        AppendV(pos, off, bg, fg, clamp, fmt, args);
        va_end(args);
    }

    const char* GetText(int idx) {
        return TextBuffer.Buf.Data + Annotations[idx].TextOffset;
    }

    void Reset() {
        Annotations.shrink(0);
        TextBuffer.Buf.shrink(0);
        Size = 0;
    }
};

struct ImPlotTag {
    ImAxis Axis;
    double Value;
    ImU32  ColorBg;
    ImU32  ColorFg;
    int    TextOffset;
};

struct ImPlotTagCollection {

    ImVector<ImPlotTag> Tags;
    ImGuiTextBuffer     TextBuffer;
    int                 Size;

    ImPlotTagCollection() { Reset(); }

    void AppendV(ImAxis axis, double value, ImU32 bg, ImU32 fg, const char* fmt, va_list args) IM_FMTLIST(6) {
        ImPlotTag tag;
        tag.Axis = axis;
        tag.Value = value;
        tag.ColorBg = bg;
        tag.ColorFg = fg;
        tag.TextOffset = TextBuffer.size();
        Tags.push_back(tag);
        TextBuffer.appendfv(fmt, args);
        const char nul[] = "";
        TextBuffer.append(nul,nul+1);
        Size++;
    }

    void Append(ImAxis axis, double value, ImU32 bg, ImU32 fg, const char* fmt, ...) IM_FMTARGS(6) {
        va_list args;
        va_start(args, fmt);
        AppendV(axis, value, bg, fg, fmt, args);
        va_end(args);
    }

    const char* GetText(int idx) {
        return TextBuffer.Buf.Data + Tags[idx].TextOffset;
    }

    void Reset() {
        Tags.shrink(0);
        TextBuffer.Buf.shrink(0);
        Size = 0;
    }
};

// Tick mark info
struct ImPlotTick
{
    double PlotPos;
    float  PixelPos;
    ImVec2 LabelSize;
    int    TextOffset;
    bool   Major;
    bool   ShowLabel;
    int    Level;
    int    Idx;

    ImPlotTick(double value, bool major, int level, bool show_label) {
        PixelPos     = 0;
        PlotPos      = value;
        Major        = major;
        ShowLabel    = show_label;
        Level        = level;
        TextOffset   = -1;
    }
};

// Collection of ticks
struct ImPlotTicker {
    ImVector<ImPlotTick> Ticks;
    ImGuiTextBuffer      TextBuffer;
    ImVec2               MaxSize;
    ImVec2               LateSize;
    int                  Levels;

    ImPlotTicker() {
        Reset();
    }

    ImPlotTick& AddTick(double value, bool major, int level, bool show_label, const char* label) {
        ImPlotTick tick(value, major, level, show_label);
        if (show_label && label != nullptr) {
            tick.TextOffset = TextBuffer.size();
            TextBuffer.append(label, label + strlen(label) + 1);
            tick.LabelSize = ImGui::CalcTextSize(TextBuffer.Buf.Data + tick.TextOffset);
        }
        return AddTick(tick);
    }

    ImPlotTick& AddTick(double value, bool major, int level, bool show_label, ImPlotFormatter formatter, void* data) {
        ImPlotTick tick(value, major, level, show_label);
        if (show_label && formatter != nullptr) {
            char buff[IMPLOT_LABEL_MAX_SIZE];
            tick.TextOffset = TextBuffer.size();
            formatter(tick.PlotPos, buff, sizeof(buff), data);
            TextBuffer.append(buff, buff + strlen(buff) + 1);
            tick.LabelSize = ImGui::CalcTextSize(TextBuffer.Buf.Data + tick.TextOffset);
        }
        return AddTick(tick);
    }

    inline ImPlotTick& AddTick(ImPlotTick tick) {
        if (tick.ShowLabel) {
            MaxSize.x     =  tick.LabelSize.x > MaxSize.x ? tick.LabelSize.x : MaxSize.x;
            MaxSize.y     =  tick.LabelSize.y > MaxSize.y ? tick.LabelSize.y : MaxSize.y;
        }
        tick.Idx = Ticks.size();
        Ticks.push_back(tick);
        return Ticks.back();
    }

    const char* GetText(int idx) const {
        return TextBuffer.Buf.Data + Ticks[idx].TextOffset;
    }

    const char* GetText(const ImPlotTick& tick) {
        return GetText(tick.Idx);
    }

    void OverrideSizeLate(const ImVec2& size) {
        LateSize.x = size.x > LateSize.x ? size.x : LateSize.x;
        LateSize.y = size.y > LateSize.y ? size.y : LateSize.y;
    }

    void Reset() {
        Ticks.shrink(0);
        TextBuffer.Buf.shrink(0);
        MaxSize = LateSize;
        LateSize = ImVec2(0,0);
        Levels = 1;
    }

    int TickCount() const {
        return Ticks.Size;
    }
};

// Axis state information that must persist after EndPlot
struct ImPlotAxis
{
    ImGuiID              ID;
    ImPlotAxisFlags      Flags;
    ImPlotAxisFlags      PreviousFlags;
    ImPlotRange          Range;
    ImPlotCond           RangeCond;
    ImPlotScale          Scale;
    ImPlotRange          FitExtents;
    ImPlotAxis*          OrthoAxis;
    ImPlotRange          ConstraintRange;
    ImPlotRange          ConstraintZoom;

    ImPlotTicker         Ticker;
    ImPlotFormatter      Formatter;
    void*                FormatterData;
    char                 FormatSpec[16];
    ImPlotLocator        Locator;

    double*              LinkedMin;
    double*              LinkedMax;

    int                  PickerLevel;
    ImPlotTime           PickerTimeMin, PickerTimeMax;

    ImPlotTransform      TransformForward;
    ImPlotTransform      TransformInverse;
    void*                TransformData;
    float                PixelMin, PixelMax;
    double               ScaleMin, ScaleMax;
    double               ScaleToPixel;
    float                Datum1, Datum2;

    ImRect               HoverRect;
    int                  LabelOffset;
    ImU32                ColorMaj, ColorMin, ColorTick, ColorTxt, ColorBg, ColorHov, ColorAct, ColorHiLi;

    bool                 Enabled;
    bool                 Vertical;
    bool                 FitThisFrame;
    bool                 HasRange;
    bool                 HasFormatSpec;
    bool                 ShowDefaultTicks;
    bool                 Hovered;
    bool                 Held;

    ImPlotAxis() {
        ID               = 0;
        Flags            = PreviousFlags = ImPlotAxisFlags_None;
        Range.Min        = 0;
        Range.Max        = 1;
        Scale            = ImPlotScale_Linear;
        TransformForward = TransformInverse = nullptr;
        TransformData    = nullptr;
        FitExtents.Min   = HUGE_VAL;
        FitExtents.Max   = -HUGE_VAL;
        OrthoAxis        = nullptr;
        ConstraintRange  = ImPlotRange(-INFINITY,INFINITY);
        ConstraintZoom   = ImPlotRange(DBL_MIN,INFINITY);
        LinkedMin        = LinkedMax = nullptr;
        PickerLevel      = 0;
        Datum1           = Datum2 = 0;
        PixelMin         = PixelMax = 0;
        LabelOffset      = -1;
        ColorMaj         = ColorMin = ColorTick = ColorTxt = ColorBg = ColorHov = ColorAct = 0;
        ColorHiLi        = IM_COL32_BLACK_TRANS;
        Formatter        = nullptr;
        FormatterData    = nullptr;
        Locator          = nullptr;
        Enabled          = Hovered = Held = FitThisFrame = HasRange = HasFormatSpec = false;
        ShowDefaultTicks = true;
    }

    inline void Reset() {
        Enabled          = false;
        Scale            = ImPlotScale_Linear;
        TransformForward = TransformInverse = nullptr;
        TransformData    = nullptr;
        LabelOffset      = -1;
        HasFormatSpec    = false;
        Formatter        = nullptr;
        FormatterData    = nullptr;
        Locator          = nullptr;
        ShowDefaultTicks = true;
        FitThisFrame     = false;
        FitExtents.Min   = HUGE_VAL;
        FitExtents.Max   = -HUGE_VAL;
        OrthoAxis        = nullptr;
        ConstraintRange  = ImPlotRange(-INFINITY,INFINITY);
        ConstraintZoom   = ImPlotRange(DBL_MIN,INFINITY);
        Ticker.Reset();
    }

    inline bool SetMin(double _min, bool force=false) {
        if (!force && IsLockedMin())
            return false;
        _min = ImConstrainNan(ImConstrainInf(_min));
        if (_min < ConstraintRange.Min)
            _min = ConstraintRange.Min;
        double z = Range.Max - _min;
        if (z < ConstraintZoom.Min)
            _min = Range.Max - ConstraintZoom.Min;
        if (z > ConstraintZoom.Max)
            _min = Range.Max - ConstraintZoom.Max;
        if (_min >= Range.Max)
            return false;
        Range.Min = _min;
        PickerTimeMin = ImPlotTime::FromDouble(Range.Min);
        UpdateTransformCache();
        return true;
    };

    inline bool SetMax(double _max, bool force=false) {
        if (!force && IsLockedMax())
            return false;
        _max = ImConstrainNan(ImConstrainInf(_max));
        if (_max > ConstraintRange.Max)
            _max = ConstraintRange.Max;
        double z = _max - Range.Min;
        if (z < ConstraintZoom.Min)
            _max = Range.Min + ConstraintZoom.Min;
        if (z > ConstraintZoom.Max)
            _max = Range.Min + ConstraintZoom.Max;
        if (_max <= Range.Min)
            return false;
        Range.Max = _max;
        PickerTimeMax = ImPlotTime::FromDouble(Range.Max);
        UpdateTransformCache();
        return true;
    };

    inline void SetRange(double v1, double v2) {
        Range.Min = ImMin(v1,v2);
        Range.Max = ImMax(v1,v2);
        Constrain();
        PickerTimeMin = ImPlotTime::FromDouble(Range.Min);
        PickerTimeMax = ImPlotTime::FromDouble(Range.Max);
        UpdateTransformCache();
    }

    inline void SetRange(const ImPlotRange& range) {
        SetRange(range.Min, range.Max);
    }

    inline void SetAspect(double unit_per_pix) {
        double new_size = unit_per_pix * PixelSize();
        double delta    = (new_size - Range.Size()) * 0.5;
        if (IsLocked())
            return;
        else if (IsLockedMin() && !IsLockedMax())
            SetRange(Range.Min, Range.Max  + 2*delta);
        else if (!IsLockedMin() && IsLockedMax())
            SetRange(Range.Min - 2*delta, Range.Max);
        else
            SetRange(Range.Min - delta, Range.Max + delta);
    }

    inline float PixelSize() const { return ImAbs(PixelMax - PixelMin); }

    inline double GetAspect() const { return Range.Size() / PixelSize(); }

    inline void Constrain() {
        Range.Min = ImConstrainNan(ImConstrainInf(Range.Min));
        Range.Max = ImConstrainNan(ImConstrainInf(Range.Max));
        if (Range.Min < ConstraintRange.Min)
            Range.Min = ConstraintRange.Min;
        if (Range.Max > ConstraintRange.Max)
            Range.Max = ConstraintRange.Max;
        double z = Range.Size();
        if (z < ConstraintZoom.Min) {
            double delta = (ConstraintZoom.Min - z) * 0.5;
            Range.Min -= delta;
            Range.Max += delta;
        }
        if (z > ConstraintZoom.Max) {
            double delta = (z - ConstraintZoom.Max) * 0.5;
            Range.Min += delta;
            Range.Max -= delta;
        }
        if (Range.Max <= Range.Min)
            Range.Max = Range.Min + DBL_EPSILON;
    }

    inline void UpdateTransformCache() {
        ScaleToPixel = (PixelMax - PixelMin) / Range.Size();
        if (TransformForward != nullptr) {
            ScaleMin = TransformForward(Range.Min, TransformData);
            ScaleMax = TransformForward(Range.Max, TransformData);
        }
        else {
            ScaleMin = Range.Min;
            ScaleMax = Range.Max;
        }
    }

    inline float PlotToPixels(double plt) const {
        if (TransformForward != nullptr) {
            double s = TransformForward(plt, TransformData);
            double t = (s - ScaleMin) / (ScaleMax - ScaleMin);
            plt      = Range.Min + Range.Size() * t;
        }
        return (float)(PixelMin + ScaleToPixel * (plt - Range.Min));
    }


    inline double PixelsToPlot(float pix) const {
        double plt = (pix - PixelMin) / ScaleToPixel + Range.Min;
        if (TransformInverse != nullptr) {
            double t = (plt - Range.Min) / Range.Size();
            double s = t * (ScaleMax - ScaleMin) + ScaleMin;
            plt = TransformInverse(s, TransformData);
        }
        return plt;
    }

    inline void ExtendFit(double v) {
        if (!ImNanOrInf(v) && v >= ConstraintRange.Min && v <= ConstraintRange.Max) {
            FitExtents.Min = v < FitExtents.Min ? v : FitExtents.Min;
            FitExtents.Max = v > FitExtents.Max ? v : FitExtents.Max;
        }
    }

    inline void ExtendFitWith(ImPlotAxis& alt, double v, double v_alt) {
        if (ImHasFlag(Flags, ImPlotAxisFlags_RangeFit) && !alt.Range.Contains(v_alt))
            return;
        if (!ImNanOrInf(v) && v >= ConstraintRange.Min && v <= ConstraintRange.Max) {
            FitExtents.Min = v < FitExtents.Min ? v : FitExtents.Min;
            FitExtents.Max = v > FitExtents.Max ? v : FitExtents.Max;
        }
    }

    inline void ApplyFit(float padding) {
        const double ext_size = FitExtents.Size() * 0.5;
        FitExtents.Min -= ext_size * padding;
        FitExtents.Max += ext_size * padding;
        if (!IsLockedMin() && !ImNanOrInf(FitExtents.Min))
            Range.Min = FitExtents.Min;
        if (!IsLockedMax() && !ImNanOrInf(FitExtents.Max))
            Range.Max = FitExtents.Max;
        if (ImAlmostEqual(Range.Min, Range.Max))  {
            Range.Max += 0.5;
            Range.Min -= 0.5;
        }
        Constrain();
        UpdateTransformCache();
    }

    inline bool HasLabel()          const { return LabelOffset != -1 && !ImHasFlag(Flags, ImPlotAxisFlags_NoLabel);                          }
    inline bool HasGridLines()      const { return !ImHasFlag(Flags, ImPlotAxisFlags_NoGridLines);                                           }
    inline bool HasTickLabels()     const { return !ImHasFlag(Flags, ImPlotAxisFlags_NoTickLabels);                                          }
    inline bool HasTickMarks()      const { return !ImHasFlag(Flags, ImPlotAxisFlags_NoTickMarks);                                           }
    inline bool WillRender()        const { return Enabled && (HasGridLines() || HasTickLabels() || HasTickMarks());                         }
    inline bool IsOpposite()        const { return ImHasFlag(Flags, ImPlotAxisFlags_Opposite);                                               }
    inline bool IsInverted()        const { return ImHasFlag(Flags, ImPlotAxisFlags_Invert);                                                 }
    inline bool IsForeground()      const { return ImHasFlag(Flags, ImPlotAxisFlags_Foreground);                                             }
    inline bool IsAutoFitting()     const { return ImHasFlag(Flags, ImPlotAxisFlags_AutoFit);                                                }
    inline bool CanInitFit()        const { return !ImHasFlag(Flags, ImPlotAxisFlags_NoInitialFit) && !HasRange && !LinkedMin && !LinkedMax; }
    inline bool IsRangeLocked()     const { return HasRange && RangeCond == ImPlotCond_Always;                                               }
    inline bool IsLockedMin()       const { return !Enabled || IsRangeLocked() || ImHasFlag(Flags, ImPlotAxisFlags_LockMin);                 }
    inline bool IsLockedMax()       const { return !Enabled || IsRangeLocked() || ImHasFlag(Flags, ImPlotAxisFlags_LockMax);                 }
    inline bool IsLocked()          const { return IsLockedMin() && IsLockedMax();                                                           }
    inline bool IsInputLockedMin()  const { return IsLockedMin() || IsAutoFitting();                                                         }
    inline bool IsInputLockedMax()  const { return IsLockedMax() || IsAutoFitting();                                                         }
    inline bool IsInputLocked()     const { return IsLocked()    || IsAutoFitting();                                                         }
    inline bool HasMenus()          const { return !ImHasFlag(Flags, ImPlotAxisFlags_NoMenus);                                               }

    inline bool IsPanLocked(bool increasing) {
        if (ImHasFlag(Flags, ImPlotAxisFlags_PanStretch)) {
            return IsInputLocked();
        }
        else {
            if (IsLockedMin() || IsLockedMax() || IsAutoFitting())
                return false;
            if (increasing)
                return Range.Max == ConstraintRange.Max;
            else
                return Range.Min == ConstraintRange.Min;
        }
    }

    void PushLinks() {
        if (LinkedMin) { *LinkedMin = Range.Min; }
        if (LinkedMax) { *LinkedMax = Range.Max; }
    }

    void PullLinks() {
        if (LinkedMin && LinkedMax) { SetRange(*LinkedMin, *LinkedMax); }
        else if (LinkedMin) { SetMin(*LinkedMin,true); }
        else if (LinkedMax) { SetMax(*LinkedMax,true); }
    }
};

// Align plots group data
struct ImPlotAlignmentData {
    bool  Vertical;
    float PadA;
    float PadB;
    float PadAMax;
    float PadBMax;
    ImPlotAlignmentData() {
        Vertical    = true;
        PadA = PadB = PadAMax = PadBMax = 0;
    }
    void Begin() { PadAMax = PadBMax = 0; }
    void Update(float& pad_a, float& pad_b, float& delta_a, float& delta_b) {
        float bak_a = pad_a; float bak_b = pad_b;
        if (PadAMax < pad_a) { PadAMax = pad_a; }
        if (PadBMax < pad_b) { PadBMax = pad_b; }
        if (pad_a < PadA)    { pad_a = PadA; delta_a = pad_a - bak_a; } else { delta_a = 0; }
        if (pad_b < PadB)    { pad_b = PadB; delta_b = pad_b - bak_b; } else { delta_b = 0; }
    }
    void End()   { PadA = PadAMax; PadB = PadBMax;      }
    void Reset() { PadA = PadB = PadAMax = PadBMax = 0; }
};

// State information for Plot items
struct ImPlotItem
{
    ImGuiID      ID;
    ImU32        Color;
    ImRect       LegendHoverRect;
    int          NameOffset;
    bool         Show;
    bool         LegendHovered;
    bool         SeenThisFrame;

    ImPlotItem() {
        ID            = 0;
        Color         = IM_COL32_WHITE;
        NameOffset    = -1;
        Show          = true;
        SeenThisFrame = false;
        LegendHovered = false;
    }

    ~ImPlotItem() { ID = 0; }
};

// Holds Legend state
struct ImPlotLegend
{
    ImPlotLegendFlags Flags;
    ImPlotLegendFlags PreviousFlags;
    ImPlotLocation    Location;
    ImPlotLocation    PreviousLocation;
    ImVec2            Scroll;
    ImVector<int>     Indices;
    ImGuiTextBuffer   Labels;
    ImRect            Rect;
    ImRect            RectClamped;
    bool              Hovered;
    bool              Held;
    bool              CanGoInside;

    ImPlotLegend() {
        Flags        = PreviousFlags = ImPlotLegendFlags_None;
        CanGoInside  = true;
        Hovered      = Held = false;
        Location     = PreviousLocation = ImPlotLocation_NorthWest;
        Scroll       = ImVec2(0,0);
    }

    void Reset() { Indices.shrink(0); Labels.Buf.shrink(0); }
};

// Holds Items and Legend data
struct ImPlotItemGroup
{
    ImGuiID            ID;
    ImPlotLegend       Legend;
    ImPool<ImPlotItem> ItemPool;
    int                ColormapIdx;

    ImPlotItemGroup() { ID = 0; ColormapIdx = 0; }

    int         GetItemCount() const             { return ItemPool.GetBufSize();                                 }
    ImGuiID     GetItemID(const char*  label_id) { return ImGui::GetID(label_id); /* GetIDWithSeed */            }
    ImPlotItem* GetItem(ImGuiID id)              { return ItemPool.GetByKey(id);                                 }
    ImPlotItem* GetItem(const char* label_id)    { return GetItem(GetItemID(label_id));                          }
    ImPlotItem* GetOrAddItem(ImGuiID id)         { return ItemPool.GetOrAddByKey(id);                            }
    ImPlotItem* GetItemByIndex(int i)            { return ItemPool.GetByIndex(i);                                }
    int         GetItemIndex(ImPlotItem* item)   { return ItemPool.GetIndex(item);                               }
    int         GetLegendCount() const           { return Legend.Indices.size();                                 }
    ImPlotItem* GetLegendItem(int i)             { return ItemPool.GetByIndex(Legend.Indices[i]);                }
    const char* GetLegendLabel(int i)            { return Legend.Labels.Buf.Data + GetLegendItem(i)->NameOffset; }
    void        Reset()                          { ItemPool.Clear(); Legend.Reset(); ColormapIdx = 0;            }
};

// Holds Plot state information that must persist after EndPlot
struct ImPlotPlot
{
    ImGuiID              ID;
    ImPlotFlags          Flags;
    ImPlotFlags          PreviousFlags;
    ImPlotLocation       MouseTextLocation;
    ImPlotMouseTextFlags MouseTextFlags;
    ImPlotAxis           Axes[ImAxis_COUNT];
    ImGuiTextBuffer      TextBuffer;
    ImPlotItemGroup      Items;
    ImAxis               CurrentX;
    ImAxis               CurrentY;
    ImRect               FrameRect;
    ImRect               CanvasRect;
    ImRect               PlotRect;
    ImRect               AxesRect;
    ImRect               SelectRect;
    ImVec2               SelectStart;
    int                  TitleOffset;
    bool                 JustCreated;
    bool                 Initialized;
    bool                 SetupLocked;
    bool                 FitThisFrame;
    bool                 Hovered;
    bool                 Held;
    bool                 Selecting;
    bool                 Selected;
    bool                 ContextLocked;

    ImPlotPlot() {
        Flags             = PreviousFlags = ImPlotFlags_None;
        for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i)
            XAxis(i).Vertical = false;
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i)
            YAxis(i).Vertical = true;
        SelectStart       = ImVec2(0,0);
        CurrentX          = ImAxis_X1;
        CurrentY          = ImAxis_Y1;
        MouseTextLocation  = ImPlotLocation_South | ImPlotLocation_East;
        MouseTextFlags     = ImPlotMouseTextFlags_None;
        TitleOffset       = -1;
        JustCreated       = true;
        Initialized = SetupLocked = FitThisFrame = false;
        Hovered = Held = Selected = Selecting = ContextLocked = false;
    }

    inline bool IsInputLocked() const {
        for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
            if (!XAxis(i).IsInputLocked())
                return false;
        }
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i) {
            if (!YAxis(i).IsInputLocked())
                return false;
        }
        return true;
    }

    inline void ClearTextBuffer() { TextBuffer.Buf.shrink(0); }

    inline void SetTitle(const char* title) {
        if (title && ImGui::FindRenderedTextEnd(title, nullptr) != title) {
            TitleOffset = TextBuffer.size();
            TextBuffer.append(title, title + strlen(title) + 1);
        }
        else {
            TitleOffset = -1;
        }
    }
    inline bool HasTitle() const { return TitleOffset != -1 && !ImHasFlag(Flags, ImPlotFlags_NoTitle); }
    inline const char* GetTitle() const { return TextBuffer.Buf.Data + TitleOffset; }

    inline       ImPlotAxis& XAxis(int i)       { return Axes[ImAxis_X1 + i]; }
    inline const ImPlotAxis& XAxis(int i) const { return Axes[ImAxis_X1 + i]; }
    inline       ImPlotAxis& YAxis(int i)       { return Axes[ImAxis_Y1 + i]; }
    inline const ImPlotAxis& YAxis(int i) const { return Axes[ImAxis_Y1 + i]; }

    inline int EnabledAxesX() {
        int cnt = 0;
        for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i)
            cnt += XAxis(i).Enabled;
        return cnt;
    }

    inline int EnabledAxesY() {
        int cnt = 0;
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i)
            cnt += YAxis(i).Enabled;
        return cnt;
    }

    inline void SetAxisLabel(ImPlotAxis& axis, const char* label) {
        if (label && ImGui::FindRenderedTextEnd(label, nullptr) != label) {
            axis.LabelOffset = TextBuffer.size();
            TextBuffer.append(label, label + strlen(label) + 1);
        }
        else {
            axis.LabelOffset = -1;
        }
    }

    inline const char* GetAxisLabel(const ImPlotAxis& axis) const { return TextBuffer.Buf.Data + axis.LabelOffset; }
};

// Holds subplot data that must persist after EndSubplot
struct ImPlotSubplot {
    ImGuiID                       ID;
    ImPlotSubplotFlags            Flags;
    ImPlotSubplotFlags            PreviousFlags;
    ImPlotItemGroup               Items;
    int                           Rows;
    int                           Cols;
    int                           CurrentIdx;
    ImRect                        FrameRect;
    ImRect                        GridRect;
    ImVec2                        CellSize;
    ImVector<ImPlotAlignmentData> RowAlignmentData;
    ImVector<ImPlotAlignmentData> ColAlignmentData;
    ImVector<float>               RowRatios;
    ImVector<float>               ColRatios;
    ImVector<ImPlotRange>         RowLinkData;
    ImVector<ImPlotRange>         ColLinkData;
    float                         TempSizes[2];
    bool                          FrameHovered;
    bool                          HasTitle;

    ImPlotSubplot() {
        ID                          = 0;
        Flags = PreviousFlags       = ImPlotSubplotFlags_None;
        Rows = Cols = CurrentIdx    = 0;
        Items.Legend.Location       = ImPlotLocation_North;
        Items.Legend.Flags          = ImPlotLegendFlags_Horizontal|ImPlotLegendFlags_Outside;
        Items.Legend.CanGoInside    = false;
        TempSizes[0] = TempSizes[1] = 0;
        FrameHovered                = false;
        HasTitle                    = false;
    }
};

// Temporary data storage for upcoming plot
struct ImPlotNextPlotData
{
    ImPlotCond  RangeCond[ImAxis_COUNT];
    ImPlotRange Range[ImAxis_COUNT];
    bool        HasRange[ImAxis_COUNT];
    bool        Fit[ImAxis_COUNT];
    double*     LinkedMin[ImAxis_COUNT];
    double*     LinkedMax[ImAxis_COUNT];

    ImPlotNextPlotData() { Reset(); }

    void Reset() {
        for (int i = 0; i < ImAxis_COUNT; ++i) {
            HasRange[i]                 = false;
            Fit[i]                      = false;
            LinkedMin[i] = LinkedMax[i] = nullptr;
        }
    }

};

// Temporary data storage for upcoming item
struct ImPlotNextItemData {
    ImVec4          Colors[5]; // ImPlotCol_Line, ImPlotCol_Fill, ImPlotCol_MarkerOutline, ImPlotCol_MarkerFill, ImPlotCol_ErrorBar
    float           LineWeight;
    ImPlotMarker    Marker;
    float           MarkerSize;
    float           MarkerWeight;
    float           FillAlpha;
    float           ErrorBarSize;
    float           ErrorBarWeight;
    float           DigitalBitHeight;
    float           DigitalBitGap;
    bool            RenderLine;
    bool            RenderFill;
    bool            RenderMarkerLine;
    bool            RenderMarkerFill;
    bool            HasHidden;
    bool            Hidden;
    ImPlotCond      HiddenCond;
    ImPlotNextItemData() { Reset(); }
    void Reset() {
        for (int i = 0; i < 5; ++i)
            Colors[i] = IMPLOT_AUTO_COL;
        LineWeight    = MarkerSize = MarkerWeight = FillAlpha = ErrorBarSize = ErrorBarWeight = DigitalBitHeight = DigitalBitGap = IMPLOT_AUTO;
        Marker        = IMPLOT_AUTO;
        HasHidden     = Hidden = false;
    }
};

// Holds state information that must persist between calls to BeginPlot()/EndPlot()
struct ImPlotContext {
    // Plot States
    ImPool<ImPlotPlot>    Plots;
    ImPool<ImPlotSubplot> Subplots;
    ImPlotPlot*           CurrentPlot;
    ImPlotSubplot*        CurrentSubplot;
    ImPlotItemGroup*      CurrentItems;
    ImPlotItem*           CurrentItem;
    ImPlotItem*           PreviousItem;

    // Tick Marks and Labels
    ImPlotTicker CTicker;

    // Annotation and Tabs
    ImPlotAnnotationCollection Annotations;
    ImPlotTagCollection        Tags;

    // Style and Colormaps
    ImPlotStyle                 Style;
    ImVector<ImGuiColorMod>     ColorModifiers;
    ImVector<ImGuiStyleMod>     StyleModifiers;
    ImPlotColormapData          ColormapData;
    ImVector<ImPlotColormap>    ColormapModifiers;

    // Time
    tm Tm;

    // Temp data for general use
    ImVector<double>   TempDouble1, TempDouble2;
    ImVector<int>      TempInt1;

    // Misc
    int                DigitalPlotItemCnt;
    int                DigitalPlotOffset;
    ImPlotNextPlotData NextPlotData;
    ImPlotNextItemData NextItemData;
    ImPlotInputMap     InputMap;
    bool               OpenContextThisFrame;
    ImGuiTextBuffer    MousePosStringBuilder;
    ImPlotItemGroup*   SortItems;

    // Align plots
    ImPool<ImPlotAlignmentData> AlignmentData;
    ImPlotAlignmentData*        CurrentAlignmentH;
    ImPlotAlignmentData*        CurrentAlignmentV;
};

//-----------------------------------------------------------------------------
// [SECTION] Internal API
// No guarantee of forward compatibility here!
//-----------------------------------------------------------------------------

namespace ImPlot {

//-----------------------------------------------------------------------------
// [SECTION] Context Utils
//-----------------------------------------------------------------------------

// Initializes an ImPlotContext
IMPLOT_API void Initialize(ImPlotContext* ctx);
// Resets an ImPlot context for the next call to BeginPlot
IMPLOT_API void ResetCtxForNextPlot(ImPlotContext* ctx);
// Resets an ImPlot context for the next call to BeginAlignedPlots
IMPLOT_API void ResetCtxForNextAlignedPlots(ImPlotContext* ctx);
// Resets an ImPlot context for the next call to BeginSubplot
IMPLOT_API void ResetCtxForNextSubplot(ImPlotContext* ctx);

//-----------------------------------------------------------------------------
// [SECTION] Plot Utils
//-----------------------------------------------------------------------------

// Gets a plot from the current ImPlotContext
IMPLOT_API ImPlotPlot* GetPlot(const char* title);
// Gets the current plot from the current ImPlotContext
IMPLOT_API ImPlotPlot* GetCurrentPlot();
// Busts the cache for every plot in the current context
IMPLOT_API void BustPlotCache();

// Shows a plot's context menu.
IMPLOT_API void ShowPlotContextMenu(ImPlotPlot& plot);

//-----------------------------------------------------------------------------
// [SECTION] Setup Utils
//-----------------------------------------------------------------------------

// Lock Setup and call SetupFinish if necessary.
static inline void SetupLock() {
    ImPlotContext& gp = *GImPlot;
    if (!gp.CurrentPlot->SetupLocked)
        SetupFinish();
    gp.CurrentPlot->SetupLocked = true;
}

//-----------------------------------------------------------------------------
// [SECTION] Subplot Utils
//-----------------------------------------------------------------------------

// Advances to next subplot
IMPLOT_API void SubplotNextCell();

// Shows a subplot's context menu.
IMPLOT_API void ShowSubplotsContextMenu(ImPlotSubplot& subplot);

//-----------------------------------------------------------------------------
// [SECTION] Item Utils
//-----------------------------------------------------------------------------

// Begins a new item. Returns false if the item should not be plotted. Pushes PlotClipRect.
IMPLOT_API bool BeginItem(const char* label_id, ImPlotItemFlags flags=0, ImPlotCol recolor_from=IMPLOT_AUTO);

// Same as above but with fitting functionality.
template <typename _Fitter>
bool BeginItemEx(const char* label_id, const _Fitter& fitter, ImPlotItemFlags flags=0, ImPlotCol recolor_from=IMPLOT_AUTO) {
    if (BeginItem(label_id, flags, recolor_from)) {
        ImPlotPlot& plot = *GetCurrentPlot();
        if (plot.FitThisFrame && !ImHasFlag(flags, ImPlotItemFlags_NoFit))
            fitter.Fit(plot.Axes[plot.CurrentX], plot.Axes[plot.CurrentY]);
        return true;
    }
    return false;
}

// Ends an item (call only if BeginItem returns true). Pops PlotClipRect.
IMPLOT_API void EndItem();

// Register or get an existing item from the current plot.
IMPLOT_API ImPlotItem* RegisterOrGetItem(const char* label_id, ImPlotItemFlags flags, bool* just_created = nullptr);
// Get a plot item from the current plot.
IMPLOT_API ImPlotItem* GetItem(const char* label_id);
// Gets the current item.
IMPLOT_API ImPlotItem* GetCurrentItem();
// Busts the cache for every item for every plot in the current context.
IMPLOT_API void BustItemCache();

//-----------------------------------------------------------------------------
// [SECTION] Axis Utils
//-----------------------------------------------------------------------------

// Returns true if any enabled axis is locked from user input.
static inline bool AnyAxesInputLocked(ImPlotAxis* axes, int count) {
    for (int i = 0; i < count; ++i) {
        if (axes[i].Enabled && axes[i].IsInputLocked())
            return true;
    }
    return false;
}

// Returns true if all enabled axes are locked from user input.
static inline bool AllAxesInputLocked(ImPlotAxis* axes, int count) {
    for (int i = 0; i < count; ++i) {
        if (axes[i].Enabled && !axes[i].IsInputLocked())
            return false;
    }
    return true;
}

static inline bool AnyAxesHeld(ImPlotAxis* axes, int count) {
    for (int i = 0; i < count; ++i) {
        if (axes[i].Enabled && axes[i].Held)
            return true;
    }
    return false;
}

static inline bool AnyAxesHovered(ImPlotAxis* axes, int count) {
    for (int i = 0; i < count; ++i) {
        if (axes[i].Enabled && axes[i].Hovered)
            return true;
    }
    return false;
}

// Returns true if the user has requested data to be fit.
static inline bool FitThisFrame() {
    return GImPlot->CurrentPlot->FitThisFrame;
}

// Extends the current plot's axes so that it encompasses a vertical line at x
static inline void FitPointX(double x) {
    ImPlotPlot& plot   = *GetCurrentPlot();
    ImPlotAxis& x_axis = plot.Axes[plot.CurrentX];
    x_axis.ExtendFit(x);
}

// Extends the current plot's axes so that it encompasses a horizontal line at y
static inline void FitPointY(double y) {
    ImPlotPlot& plot   = *GetCurrentPlot();
    ImPlotAxis& y_axis = plot.Axes[plot.CurrentY];
    y_axis.ExtendFit(y);
}

// Extends the current plot's axes so that it encompasses point p
static inline void FitPoint(const ImPlotPoint& p) {
    ImPlotPlot& plot   = *GetCurrentPlot();
    ImPlotAxis& x_axis = plot.Axes[plot.CurrentX];
    ImPlotAxis& y_axis = plot.Axes[plot.CurrentY];
    x_axis.ExtendFitWith(y_axis, p.x, p.y);
    y_axis.ExtendFitWith(x_axis, p.y, p.x);
}

// Returns true if two ranges overlap
static inline bool RangesOverlap(const ImPlotRange& r1, const ImPlotRange& r2)
{ return r1.Min <= r2.Max && r2.Min <= r1.Max; }

// Shows an axis's context menu.
IMPLOT_API void ShowAxisContextMenu(ImPlotAxis& axis, ImPlotAxis* equal_axis, bool time_allowed = false);

//-----------------------------------------------------------------------------
// [SECTION] Legend Utils
//-----------------------------------------------------------------------------

// Gets the position of an inner rect that is located inside of an outer rect according to an ImPlotLocation and padding amount.
IMPLOT_API ImVec2 GetLocationPos(const ImRect& outer_rect, const ImVec2& inner_size, ImPlotLocation location, const ImVec2& pad = ImVec2(0,0));
// Calculates the bounding box size of a legend _before_ clipping.
IMPLOT_API ImVec2 CalcLegendSize(ImPlotItemGroup& items, const ImVec2& pad, const ImVec2& spacing, bool vertical);
// Clips calculated legend size
IMPLOT_API bool ClampLegendRect(ImRect& legend_rect, const ImRect& outer_rect, const ImVec2& pad);
// Renders legend entries into a bounding box
IMPLOT_API bool ShowLegendEntries(ImPlotItemGroup& items, const ImRect& legend_bb, bool interactable, const ImVec2& pad, const ImVec2& spacing, bool vertical, ImDrawList& DrawList);
// Shows an alternate legend for the plot identified by #title_id, outside of the plot frame (can be called before or after of Begin/EndPlot but must occur in the same ImGui window! This is not thoroughly tested nor scrollable!).
IMPLOT_API void ShowAltLegend(const char* title_id, bool vertical = true, const ImVec2 size = ImVec2(0,0), bool interactable = true);
// Shows a legend's context menu.
IMPLOT_API bool ShowLegendContextMenu(ImPlotLegend& legend, bool visible);

//-----------------------------------------------------------------------------
// [SECTION] Label Utils
//-----------------------------------------------------------------------------

// Create a a string label for a an axis value
IMPLOT_API void LabelAxisValue(const ImPlotAxis& axis, double value, char* buff, int size, bool round = false);

//-----------------------------------------------------------------------------
// [SECTION] Styling Utils
//-----------------------------------------------------------------------------

// Get styling data for next item (call between Begin/EndItem)
static inline const ImPlotNextItemData& GetItemData() { return GImPlot->NextItemData; }

// Returns true if a color is set to be automatically determined
static inline bool IsColorAuto(const ImVec4& col) { return col.w == -1; }
// Returns true if a style color is set to be automatically determined
static inline bool IsColorAuto(ImPlotCol idx) { return IsColorAuto(GImPlot->Style.Colors[idx]); }
// Returns the automatically deduced style color
IMPLOT_API ImVec4 GetAutoColor(ImPlotCol idx);

// Returns the style color whether it is automatic or custom set
static inline ImVec4 GetStyleColorVec4(ImPlotCol idx) { return IsColorAuto(idx) ? GetAutoColor(idx) : GImPlot->Style.Colors[idx]; }
static inline ImU32  GetStyleColorU32(ImPlotCol idx)  { return ImGui::ColorConvertFloat4ToU32(GetStyleColorVec4(idx)); }

// Draws vertical text. The position is the bottom left of the text rect.
IMPLOT_API void AddTextVertical(ImDrawList *DrawList, ImVec2 pos, ImU32 col, const char* text_begin, const char* text_end = nullptr);
// Draws multiline horizontal text centered.
IMPLOT_API void AddTextCentered(ImDrawList* DrawList, ImVec2 top_center, ImU32 col, const char* text_begin, const char* text_end = nullptr);
// Calculates the size of vertical text
static inline ImVec2 CalcTextSizeVertical(const char *text) {
    ImVec2 sz = ImGui::CalcTextSize(text);
    return ImVec2(sz.y, sz.x);
}
// Returns white or black text given background color
static inline ImU32 CalcTextColor(const ImVec4& bg) { return (bg.x * 0.299f + bg.y * 0.587f + bg.z * 0.114f) > 0.5f ? IM_COL32_BLACK : IM_COL32_WHITE; }
static inline ImU32 CalcTextColor(ImU32 bg)         { return CalcTextColor(ImGui::ColorConvertU32ToFloat4(bg)); }
// Lightens or darkens a color for hover
static inline ImU32 CalcHoverColor(ImU32 col)       {  return ImMixU32(col, CalcTextColor(col), 32); }

// Clamps a label position so that it fits a rect defined by Min/Max
static inline ImVec2 ClampLabelPos(ImVec2 pos, const ImVec2& size, const ImVec2& Min, const ImVec2& Max) {
    if (pos.x < Min.x)              pos.x = Min.x;
    if (pos.y < Min.y)              pos.y = Min.y;
    if ((pos.x + size.x) > Max.x)   pos.x = Max.x - size.x;
    if ((pos.y + size.y) > Max.y)   pos.y = Max.y - size.y;
    return pos;
}

// Returns a color from the Color map given an index >= 0 (modulo will be performed).
IMPLOT_API ImU32  GetColormapColorU32(int idx, ImPlotColormap cmap);
// Returns the next unused colormap color and advances the colormap. Can be used to skip colors if desired.
IMPLOT_API ImU32  NextColormapColorU32();
// Linearly interpolates a color from the current colormap given t between 0 and 1.
IMPLOT_API ImU32  SampleColormapU32(float t, ImPlotColormap cmap);

// Render a colormap bar
IMPLOT_API void RenderColorBar(const ImU32* colors, int size, ImDrawList& DrawList, const ImRect& bounds, bool vert, bool reversed, bool continuous);

//-----------------------------------------------------------------------------
// [SECTION] Math and Misc Utils
//-----------------------------------------------------------------------------

// Rounds x to powers of 2,5 and 10 for generating axis labels (from Graphics Gems 1 Chapter 11.2)
IMPLOT_API double NiceNum(double x, bool round);
// Computes order of magnitude of double.
static inline int OrderOfMagnitude(double val) { return val == 0 ? 0 : (int)(floor(log10(fabs(val)))); }
// Returns the precision required for a order of magnitude.
static inline int OrderToPrecision(int order) { return order > 0 ? 0 : 1 - order; }
// Returns a floating point precision to use given a value
static inline int Precision(double val) { return OrderToPrecision(OrderOfMagnitude(val)); }
// Round a value to a given precision
static inline double RoundTo(double val, int prec) { double p = pow(10,(double)prec); return floor(val*p+0.5)/p; }

// Returns the intersection point of two lines A and B (assumes they are not parallel!)
static inline ImVec2 Intersection(const ImVec2& a1, const ImVec2& a2, const ImVec2& b1, const ImVec2& b2) {
    float v1 = (a1.x * a2.y - a1.y * a2.x);  float v2 = (b1.x * b2.y - b1.y * b2.x);
    float v3 = ((a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x));
    return ImVec2((v1 * (b1.x - b2.x) - v2 * (a1.x - a2.x)) / v3, (v1 * (b1.y - b2.y) - v2 * (a1.y - a2.y)) / v3);
}

// Fills a buffer with n samples linear interpolated from vmin to vmax
template <typename T>
void FillRange(ImVector<T>& buffer, int n, T vmin, T vmax) {
    buffer.resize(n);
    T step = (vmax - vmin) / (n - 1);
    for (int i = 0; i < n; ++i) {
        buffer[i] = vmin + i * step;
    }
}

// Calculate histogram bin counts and widths
template <typename T>
static inline void CalculateBins(const T* values, int count, ImPlotBin meth, const ImPlotRange& range, int& bins_out, double& width_out) {
    switch (meth) {
        case ImPlotBin_Sqrt:
            bins_out  = (int)ceil(sqrt(count));
            break;
        case ImPlotBin_Sturges:
            bins_out  = (int)ceil(1.0 + log2(count));
            break;
        case ImPlotBin_Rice:
            bins_out  = (int)ceil(2 * cbrt(count));
            break;
        case ImPlotBin_Scott:
            width_out = 3.49 * ImStdDev(values, count) / cbrt(count);
            bins_out  = (int)round(range.Size() / width_out);
            break;
    }
    width_out = range.Size() / bins_out;
}

//-----------------------------------------------------------------------------
// Time Utils
//-----------------------------------------------------------------------------

// Returns true if year is leap year (366 days long)
static inline bool IsLeapYear(int year) {
    return  year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}
// Returns the number of days in a month, accounting for Feb. leap years. #month is zero indexed.
static inline int GetDaysInMonth(int year, int month) {
    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return  days[month] + (int)(month == 1 && IsLeapYear(year));
}

// Make a UNIX timestamp from a tm struct expressed in UTC time (i.e. GMT timezone).
IMPLOT_API ImPlotTime MkGmtTime(struct tm *ptm);
// Make a tm struct expressed in UTC time (i.e. GMT timezone) from a UNIX timestamp.
IMPLOT_API tm* GetGmtTime(const ImPlotTime& t, tm* ptm);

// Make a UNIX timestamp from a tm struct expressed in local time.
IMPLOT_API ImPlotTime MkLocTime(struct tm *ptm);
// Make a tm struct expressed in local time from a UNIX timestamp.
IMPLOT_API tm* GetLocTime(const ImPlotTime& t, tm* ptm);

// NB: The following functions only work if there is a current ImPlotContext because the
// internal tm struct is owned by the context! They are aware of ImPlotStyle.UseLocalTime.

// Make a timestamp from time components.
// year[1970-3000], month[0-11], day[1-31], hour[0-23], min[0-59], sec[0-59], us[0,999999]
IMPLOT_API ImPlotTime MakeTime(int year, int month = 0, int day = 1, int hour = 0, int min = 0, int sec = 0, int us = 0);
// Get year component from timestamp [1970-3000]
IMPLOT_API int GetYear(const ImPlotTime& t);

// Adds or subtracts time from a timestamp. #count > 0 to add, < 0 to subtract.
IMPLOT_API ImPlotTime AddTime(const ImPlotTime& t, ImPlotTimeUnit unit, int count);
// Rounds a timestamp down to nearest unit.
IMPLOT_API ImPlotTime FloorTime(const ImPlotTime& t, ImPlotTimeUnit unit);
// Rounds a timestamp up to the nearest unit.
IMPLOT_API ImPlotTime CeilTime(const ImPlotTime& t, ImPlotTimeUnit unit);
// Rounds a timestamp up or down to the nearest unit.
IMPLOT_API ImPlotTime RoundTime(const ImPlotTime& t, ImPlotTimeUnit unit);
// Combines the date of one timestamp with the time-of-day of another timestamp.
IMPLOT_API ImPlotTime CombineDateTime(const ImPlotTime& date_part, const ImPlotTime& time_part);

// Formats the time part of timestamp t into a buffer according to #fmt
IMPLOT_API int FormatTime(const ImPlotTime& t, char* buffer, int size, ImPlotTimeFmt fmt, bool use_24_hr_clk);
// Formats the date part of timestamp t into a buffer according to #fmt
IMPLOT_API int FormatDate(const ImPlotTime& t, char* buffer, int size, ImPlotDateFmt fmt, bool use_iso_8601);
// Formats the time and/or date parts of a timestamp t into a buffer according to #fmt
IMPLOT_API int FormatDateTime(const ImPlotTime& t, char* buffer, int size, ImPlotDateTimeSpec fmt);

// Shows a date picker widget block (year/month/day).
// #level = 0 for day, 1 for month, 2 for year. Modified by user interaction.
// #t will be set when a day is clicked and the function will return true.
// #t1 and #t2 are optional dates to highlight.
IMPLOT_API bool ShowDatePicker(const char* id, int* level, ImPlotTime* t, const ImPlotTime* t1 = nullptr, const ImPlotTime* t2 = nullptr);
// Shows a time picker widget block (hour/min/sec).
// #t will be set when a new hour, minute, or sec is selected or am/pm is toggled, and the function will return true.
IMPLOT_API bool ShowTimePicker(const char* id, ImPlotTime* t);

//-----------------------------------------------------------------------------
// [SECTION] Transforms
//-----------------------------------------------------------------------------

static inline double TransformForward_Log10(double v, void*) {
    v = v <= 0.0 ? DBL_MIN : v;
    return ImLog10(v);
}

static inline double TransformInverse_Log10(double v, void*) {
    return ImPow(10, v);
}

static inline double TransformForward_SymLog(double v, void*) {
    return 2.0 * ImAsinh(v / 2.0);
}

static inline double TransformInverse_SymLog(double v, void*) {
    return 2.0 * ImSinh(v / 2.0);
}

static inline double TransformForward_Logit(double v, void*) {
    v = ImClamp(v, DBL_MIN, 1.0 - DBL_EPSILON);
    return ImLog10(v / (1 - v));
}

static inline double TransformInverse_Logit(double v, void*) {
    return 1.0 / (1.0 + ImPow(10,-v));
}

//-----------------------------------------------------------------------------
// [SECTION] Formatters
//-----------------------------------------------------------------------------

static inline int Formatter_Default(double value, char* buff, int size, void* data) {
    char* fmt = (char*)data;
    return ImFormatString(buff, size, fmt, value);
}

static inline int Formatter_Logit(double value, char* buff, int size, void*) {
    if (value == 0.5)
        return ImFormatString(buff,size,"1/2");
    else if (value < 0.5)
        return ImFormatString(buff,size,"%g", value);
    else
        return ImFormatString(buff,size,"1 - %g", 1 - value);
}

struct Formatter_Time_Data {
    ImPlotTime Time;
    ImPlotDateTimeSpec Spec;
    ImPlotFormatter UserFormatter;
    void* UserFormatterData;
};

static inline int Formatter_Time(double, char* buff, int size, void* data) {
    Formatter_Time_Data* ftd = (Formatter_Time_Data*)data;
    return FormatDateTime(ftd->Time, buff, size, ftd->Spec);
}

//------------------------------------------------------------------------------
// [SECTION] Locator
//------------------------------------------------------------------------------

void Locator_Default(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data);
void Locator_Time(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data);
void Locator_Log10(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data);
void Locator_SymLog(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data);

} // namespace ImPlot

// MIT License

// Copyright (c) 2023 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.17

/*

API BREAKING CHANGES
====================
Occasionally introducing changes that are breaking the API. We try to make the breakage minor and easy to fix.
Below is a change-log of API breaking changes only. If you are using one of the functions listed, expect to have to fix some code.
When you are not sure about a old symbol or function name, try using the Search/Find function of your IDE to look for comments or references in all implot files.
You can read releases logs https://github.com/epezent/implot/releases for more details.

- 2023/08/20 (0.17) - ImPlotFlags_NoChild was removed as child windows are no longer needed to capture scroll. You can safely remove this flag if you were using it.
- 2023/06/26 (0.15) - Various build fixes related to updates in Dear ImGui internals.
- 2022/11/25 (0.15) - Make PlotText honor ImPlotItemFlags_NoFit.
- 2022/06/19 (0.14) - The signature of ColormapScale has changed to accommodate a new ImPlotColormapScaleFlags parameter
- 2022/06/17 (0.14) - **IMPORTANT** All PlotX functions now take an ImPlotX_Flags `flags` parameter. Where applicable, it is located before the existing `offset` and `stride` parameters.
                      If you were providing offset and stride values, you will need to update your function call to include a `flags` value. If you fail to do this, you will likely see
                      unexpected results or crashes without a compiler warning since these three are all default args. We apologize for the inconvenience, but this was a necessary evil.
                    - PlotBarsH has been removed; use PlotBars + ImPlotBarsFlags_Horizontal instead
                    - PlotErrorBarsH has been removed; use PlotErrorBars + ImPlotErrorBarsFlags_Horizontal
                    - PlotHistogram/PlotHistogram2D signatures changed; `cumulative`, `density`, and `outliers` options now specified via ImPlotHistogramFlags
                    - PlotPieChart signature changed; `normalize` option now specified via ImPlotPieChartFlags
                    - PlotText signature changes; `vertical` option now specified via `ImPlotTextFlags_Vertical`
                    - `PlotVLines` and `PlotHLines` replaced with `PlotInfLines` (+ ImPlotInfLinesFlags_Horizontal )
                    - arguments of ImPlotGetter have been reversed to be consistent with other API callbacks
                    - SetupAxisScale + ImPlotScale have replaced ImPlotAxisFlags_LogScale and ImPlotAxisFlags_Time flags
                    - ImPlotFormatters should now return an int indicating the size written
                    - the signature of ImPlotGetter has been reversed so that void* user_data is the last argument and consistent with other callbacks
- 2021/10/19 (0.13) - MAJOR API OVERHAUL! See #168 and #272
                    - TRIVIAL RENAME:
                      - ImPlotLimits                              -> ImPlotRect
                      - ImPlotYAxis_                              -> ImAxis_
                      - SetPlotYAxis                              -> SetAxis
                      - BeginDragDropTarget                       -> BeginDragDropTargetPlot
                      - BeginDragDropSource                       -> BeginDragDropSourcePlot
                      - ImPlotFlags_NoMousePos                    -> ImPlotFlags_NoMouseText
                      - SetNextPlotLimits                         -> SetNextAxesLimits
                      - SetMouseTextLocation                      -> SetupMouseText
                    - SIGNATURE MODIFIED:
                      - PixelsToPlot/PlotToPixels                 -> added optional X-Axis arg
                      - GetPlotMousePos                           -> added optional X-Axis arg
                      - GetPlotLimits                             -> added optional X-Axis arg
                      - GetPlotSelection                          -> added optional X-Axis arg
                      - DragLineX/Y/DragPoint                     -> now takes int id; removed labels (render with Annotation/Tag instead)
                    - REPLACED:
                      - IsPlotXAxisHovered/IsPlotXYAxisHovered    -> IsAxisHovered(ImAxis)
                      - BeginDragDropTargetX/BeginDragDropTargetY -> BeginDragDropTargetAxis(ImAxis)
                      - BeginDragDropSourceX/BeginDragDropSourceY -> BeginDragDropSourceAxis(ImAxis)
                      - ImPlotCol_XAxis, ImPlotCol_YAxis1, etc.   -> ImPlotCol_AxisText (push/pop this around SetupAxis to style individual axes)
                      - ImPlotCol_XAxisGrid, ImPlotCol_Y1AxisGrid -> ImPlotCol_AxisGrid (push/pop this around SetupAxis to style individual axes)
                      - SetNextPlotLimitsX/Y                      -> SetNextAxisLimits(ImAxis)
                      - LinkNextPlotLimits                        -> SetNextAxisLinks(ImAxis)
                      - FitNextPlotAxes                           -> SetNextAxisToFit(ImAxis)/SetNextAxesToFit
                      - SetLegendLocation                         -> SetupLegend
                      - ImPlotFlags_NoHighlight                   -> ImPlotLegendFlags_NoHighlight
                      - ImPlotOrientation                         -> ImPlotLegendFlags_Horizontal
                      - Annotate                                  -> Annotation
                    - REMOVED:
                      - GetPlotQuery, SetPlotQuery, IsPlotQueried -> use DragRect
                      - SetNextPlotTicksX, SetNextPlotTicksY      -> use SetupAxisTicks
                      - SetNextPlotFormatX, SetNextPlotFormatY    -> use SetupAxisFormat
                      - AnnotateClamped                           -> use Annotation(bool clamp = true)
                    - OBSOLETED:
                      - BeginPlot (original signature)            -> use simplified signature + Setup API
- 2021/07/30 (0.12) - The offset argument of `PlotXG` functions was been removed. Implement offsetting in your getter callback instead.
- 2021/03/08 (0.9)  - SetColormap and PushColormap(ImVec4*) were removed. Use AddColormap for custom colormap support. LerpColormap was changed to SampleColormap.
                      ShowColormapScale was changed to ColormapScale and requires additional arguments.
- 2021/03/07 (0.9)  - The signature of ShowColormapScale was modified to accept a ImVec2 size.
- 2021/02/28 (0.9)  - BeginLegendDragDropSource was changed to BeginDragDropSourceItem with a number of other drag and drop improvements.
- 2021/01/18 (0.9)  - The default behavior for opening context menus was change from double right-click to single right-click. ImPlotInputMap and related functions were moved
                      to implot_internal.h due to its immaturity.
- 2020/10/16 (0.8)  - ImPlotStyleVar_InfoPadding was changed to ImPlotStyleVar_MousePosPadding
- 2020/09/10 (0.8)  - The single array versions of PlotLine, PlotScatter, PlotStems, and PlotShaded were given additional arguments for x-scale and x0.
- 2020/09/07 (0.8)  - Plotting functions which accept a custom getter function pointer have been post-fixed with a G (e.g. PlotLineG)
- 2020/09/06 (0.7)  - Several flags under ImPlotFlags and ImPlotAxisFlags were inverted (e.g. ImPlotFlags_Legend -> ImPlotFlags_NoLegend) so that the default flagset
                      is simply 0. This more closely matches ImGui's style and makes it easier to enable non-default but commonly used flags (e.g. ImPlotAxisFlags_Time).
- 2020/08/28 (0.5)  - ImPlotMarker_ can no longer be combined with bitwise OR, |. This features caused unecessary slow-down, and almost no one used it.
- 2020/08/25 (0.5)  - ImPlotAxisFlags_Scientific was removed. Logarithmic axes automatically uses scientific notation.
- 2020/08/17 (0.5)  - PlotText was changed so that text is centered horizontally and vertically about the desired point.
- 2020/08/16 (0.5)  - An ImPlotContext must be explicitly created and destroyed now with `CreateContext` and `DestroyContext`. Previously, the context was statically initialized in this source file.
- 2020/06/13 (0.4)  - The flags `ImPlotAxisFlag_Adaptive` and `ImPlotFlags_Cull` were removed. Both are now done internally by default.
- 2020/06/03 (0.3)  - The signature and behavior of PlotPieChart was changed so that data with sum less than 1 can optionally be normalized. The label format can now be specified as well.
- 2020/06/01 (0.3)  - SetPalette was changed to `SetColormap` for consistency with other plotting libraries. `RestorePalette` was removed. Use `SetColormap(ImPlotColormap_Default)`.
- 2020/05/31 (0.3)  - Plot functions taking custom ImVec2* getters were removed. Use the ImPlotPoint* getter versions instead.
- 2020/05/29 (0.3)  - The signature of ImPlotLimits::Contains was changed to take two doubles instead of ImVec2
- 2020/05/16 (0.2)  - All plotting functions were reverted to being prefixed with "Plot" to maintain a consistent VerbNoun style. `Plot` was split into `PlotLine`
                      and `PlotScatter` (however, `PlotLine` can still be used to plot scatter points as `Plot` did before.). `Bar` is not `PlotBars`, to indicate
                      that multiple bars will be plotted.
- 2020/05/13 (0.2)  - `ImMarker` was change to `ImPlotMarker` and `ImAxisFlags` was changed to `ImPlotAxisFlags`.
- 2020/05/11 (0.2)  - `ImPlotFlags_Selection` was changed to `ImPlotFlags_BoxSelect`
- 2020/05/11 (0.2)  - The namespace ImGui:: was replaced with ImPlot::. As a result, the following additional changes were made:
                      - Functions that were prefixed or decorated with the word "Plot" have been truncated. E.g., `ImGui::PlotBars` is now just `ImPlot::Bar`.
                        It should be fairly obvious what was what.
                      - Some functions have been given names that would have otherwise collided with the ImGui namespace. This has been done to maintain a consistent
                        style with ImGui. E.g., 'ImGui::PushPlotStyleVar` is now 'ImPlot::PushStyleVar'.
- 2020/05/10 (0.2)  - The following function/struct names were changes:
                     - ImPlotRange       -> ImPlotLimits
                     - GetPlotRange()    -> GetPlotLimits()
                     - SetNextPlotRange  -> SetNextPlotLimits
                     - SetNextPlotRangeX -> SetNextPlotLimitsX
                     - SetNextPlotRangeY -> SetNextPlotLimitsY
- 2020/05/10 (0.2)  - Plot queries are pixel based by default. Query rects that maintain relative plot position have been removed. This was done to support multi-y-axis.

*/

//#define IMGUI_DEFINE_MATH_OPERATORS
//#include "implot.h"
//#include "implot_internal.h"

#include <stdlib.h>

// Support for pre-1.82 versions. Users on 1.82+ can use 0 (default) flags to mean "all corners" but in order to support older versions we are more explicit.
#if (IMGUI_VERSION_NUM < 18102) && !defined(ImDrawFlags_RoundCornersAll)
#define ImDrawFlags_RoundCornersAll ImDrawCornerFlags_All
#endif

// Support for pre-1.89.7 versions.
#if (IMGUI_VERSION_NUM < 18966)
#define ImGuiButtonFlags_AllowOverlap ImGuiButtonFlags_AllowItemOverlap
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wformat-nonliteral"  // warning: format string is not a string literal
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat-nonliteral"    // warning: format not a string literal, format string not checked
#endif

// Global plot context
#ifndef GImPlot
ImPlotContext* GImPlot = nullptr;
#endif

//-----------------------------------------------------------------------------
// Struct Implementations
//-----------------------------------------------------------------------------

ImPlotInputMap::ImPlotInputMap() {
    ImPlot::MapInputDefault(this);
}

ImPlotStyle::ImPlotStyle() {

    LineWeight         = 1;
    Marker             = ImPlotMarker_None;
    MarkerSize         = 4;
    MarkerWeight       = 1;
    FillAlpha          = 1;
    ErrorBarSize       = 5;
    ErrorBarWeight     = 1.5f;
    DigitalBitHeight   = 8;
    DigitalBitGap      = 4;

    PlotBorderSize     = 1;
    MinorAlpha         = 0.25f;
    MajorTickLen       = ImVec2(10,10);
    MinorTickLen       = ImVec2(5,5);
    MajorTickSize      = ImVec2(1,1);
    MinorTickSize      = ImVec2(1,1);
    MajorGridSize      = ImVec2(1,1);
    MinorGridSize      = ImVec2(1,1);
    PlotPadding        = ImVec2(10,10);
    LabelPadding       = ImVec2(5,5);
    LegendPadding      = ImVec2(10,10);
    LegendInnerPadding = ImVec2(5,5);
    LegendSpacing      = ImVec2(5,0);
    MousePosPadding    = ImVec2(10,10);
    AnnotationPadding  = ImVec2(2,2);
    FitPadding         = ImVec2(0,0);
    PlotDefaultSize    = ImVec2(400,300);
    PlotMinSize        = ImVec2(200,150);

    ImPlot::StyleColorsAuto(this);

    Colormap = ImPlotColormap_Deep;

    UseLocalTime     = false;
    Use24HourClock   = false;
    UseISO8601       = false;
}

//-----------------------------------------------------------------------------
// Style
//-----------------------------------------------------------------------------

namespace ImPlot {

const char* GetStyleColorName(ImPlotCol col) {
    static const char* col_names[ImPlotCol_COUNT] = {
        "Line",
        "Fill",
        "MarkerOutline",
        "MarkerFill",
        "ErrorBar",
        "FrameBg",
        "PlotBg",
        "PlotBorder",
        "LegendBg",
        "LegendBorder",
        "LegendText",
        "TitleText",
        "InlayText",
        "AxisText",
        "AxisGrid",
        "AxisTick",
        "AxisBg",
        "AxisBgHovered",
        "AxisBgActive",
        "Selection",
        "Crosshairs"
    };
    return col_names[col];
}

const char* GetMarkerName(ImPlotMarker marker) {
    switch (marker) {
        case ImPlotMarker_None:     return "None";
        case ImPlotMarker_Circle:   return "Circle";
        case ImPlotMarker_Square:   return "Square";
        case ImPlotMarker_Diamond:  return "Diamond";
        case ImPlotMarker_Up:       return "Up";
        case ImPlotMarker_Down:     return "Down";
        case ImPlotMarker_Left:     return "Left";
        case ImPlotMarker_Right:    return "Right";
        case ImPlotMarker_Cross:    return "Cross";
        case ImPlotMarker_Plus:     return "Plus";
        case ImPlotMarker_Asterisk: return "Asterisk";
        default:                    return "";
    }
}

ImVec4 GetAutoColor(ImPlotCol idx) {
    ImVec4 col(0,0,0,1);
    switch(idx) {
        case ImPlotCol_Line:          return col; // these are plot dependent!
        case ImPlotCol_Fill:          return col; // these are plot dependent!
        case ImPlotCol_MarkerOutline: return col; // these are plot dependent!
        case ImPlotCol_MarkerFill:    return col; // these are plot dependent!
        case ImPlotCol_ErrorBar:      return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_FrameBg:       return ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
        case ImPlotCol_PlotBg:        return ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
        case ImPlotCol_PlotBorder:    return ImGui::GetStyleColorVec4(ImGuiCol_Border);
        case ImPlotCol_LegendBg:      return ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
        case ImPlotCol_LegendBorder:  return GetStyleColorVec4(ImPlotCol_PlotBorder);
        case ImPlotCol_LegendText:    return GetStyleColorVec4(ImPlotCol_InlayText);
        case ImPlotCol_TitleText:     return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_InlayText:     return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_AxisText:      return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        case ImPlotCol_AxisGrid:      return GetStyleColorVec4(ImPlotCol_AxisText) * ImVec4(1,1,1,0.25f);
        case ImPlotCol_AxisTick:      return GetStyleColorVec4(ImPlotCol_AxisGrid);
        case ImPlotCol_AxisBg:        return ImVec4(0,0,0,0);
        case ImPlotCol_AxisBgHovered: return ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
        case ImPlotCol_AxisBgActive:  return ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
        case ImPlotCol_Selection:     return ImVec4(1,1,0,1);
        case ImPlotCol_Crosshairs:    return GetStyleColorVec4(ImPlotCol_PlotBorder);
        default: return col;
    }
}

struct ImPlotStyleVarInfo {
    ImGuiDataType   Type;
    ImU32           Count;
    ImU32           Offset;
    void*           GetVarPtr(ImPlotStyle* style) const { return (void*)((unsigned char*)style + Offset); }
};

static const ImPlotStyleVarInfo GPlotStyleVarInfo[] =
{
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, LineWeight)         }, // ImPlotStyleVar_LineWeight
    { ImGuiDataType_S32,   1, (ImU32)offsetof(ImPlotStyle, Marker)             }, // ImPlotStyleVar_Marker
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, MarkerSize)         }, // ImPlotStyleVar_MarkerSize
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, MarkerWeight)       }, // ImPlotStyleVar_MarkerWeight
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, FillAlpha)          }, // ImPlotStyleVar_FillAlpha
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, ErrorBarSize)       }, // ImPlotStyleVar_ErrorBarSize
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, ErrorBarWeight)     }, // ImPlotStyleVar_ErrorBarWeight
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, DigitalBitHeight)   }, // ImPlotStyleVar_DigitalBitHeight
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, DigitalBitGap)      }, // ImPlotStyleVar_DigitalBitGap

    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, PlotBorderSize)     }, // ImPlotStyleVar_PlotBorderSize
    { ImGuiDataType_Float, 1, (ImU32)offsetof(ImPlotStyle, MinorAlpha)         }, // ImPlotStyleVar_MinorAlpha
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MajorTickLen)       }, // ImPlotStyleVar_MajorTickLen
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MinorTickLen)       }, // ImPlotStyleVar_MinorTickLen
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MajorTickSize)      }, // ImPlotStyleVar_MajorTickSize
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MinorTickSize)      }, // ImPlotStyleVar_MinorTickSize
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MajorGridSize)      }, // ImPlotStyleVar_MajorGridSize
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MinorGridSize)      }, // ImPlotStyleVar_MinorGridSize
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, PlotPadding)        }, // ImPlotStyleVar_PlotPadding
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, LabelPadding)       }, // ImPlotStyleVar_LabelPaddine
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, LegendPadding)      }, // ImPlotStyleVar_LegendPadding
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, LegendInnerPadding) }, // ImPlotStyleVar_LegendInnerPadding
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, LegendSpacing)      }, // ImPlotStyleVar_LegendSpacing

    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, MousePosPadding)    }, // ImPlotStyleVar_MousePosPadding
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, AnnotationPadding)  }, // ImPlotStyleVar_AnnotationPadding
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, FitPadding)         }, // ImPlotStyleVar_FitPadding
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, PlotDefaultSize)    }, // ImPlotStyleVar_PlotDefaultSize
    { ImGuiDataType_Float, 2, (ImU32)offsetof(ImPlotStyle, PlotMinSize)        }  // ImPlotStyleVar_PlotMinSize
};

static const ImPlotStyleVarInfo* GetPlotStyleVarInfo(ImPlotStyleVar idx) {
    IM_ASSERT(idx >= 0 && idx < ImPlotStyleVar_COUNT);
    IM_ASSERT(IM_ARRAYSIZE(GPlotStyleVarInfo) == ImPlotStyleVar_COUNT);
    return &GPlotStyleVarInfo[idx];
}

//-----------------------------------------------------------------------------
// Generic Helpers
//-----------------------------------------------------------------------------

void AddTextVertical(ImDrawList *DrawList, ImVec2 pos, ImU32 col, const char *text_begin, const char* text_end) {
    // the code below is based loosely on ImFont::RenderText
    if (!text_end)
        text_end = text_begin + strlen(text_begin);
    ImGuiContext& g = *GImGui;
    ImFont* font = g.Font;
    // Align to be pixel perfect
    pos.x = ImFloor(pos.x);
    pos.y = ImFloor(pos.y);
    const float scale = g.FontSize / font->FontSize;
    const char* s = text_begin;
    int chars_exp = (int)(text_end - s);
    int chars_rnd = 0;
    const int vtx_count_max = chars_exp * 4;
    const int idx_count_max = chars_exp * 6;
    DrawList->PrimReserve(idx_count_max, vtx_count_max);
    while (s < text_end) {
        unsigned int c = (unsigned int)*s;
        if (c < 0x80) {
            s += 1;
        }
        else {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0) // Malformed UTF-8?
                break;
        }
        const ImFontGlyph * glyph = font->FindGlyph((ImWchar)c);
        if (glyph == nullptr) {
            continue;
        }
        DrawList->PrimQuadUV(pos + ImVec2(glyph->Y0, -glyph->X0) * scale, pos + ImVec2(glyph->Y0, -glyph->X1) * scale,
                             pos + ImVec2(glyph->Y1, -glyph->X1) * scale, pos + ImVec2(glyph->Y1, -glyph->X0) * scale,
                             ImVec2(glyph->U0, glyph->V0), ImVec2(glyph->U1, glyph->V0),
                             ImVec2(glyph->U1, glyph->V1), ImVec2(glyph->U0, glyph->V1),
                             col);
        pos.y -= glyph->AdvanceX * scale;
        chars_rnd++;
    }
    // Give back unused vertices
    int chars_skp = chars_exp-chars_rnd;
    DrawList->PrimUnreserve(chars_skp*6, chars_skp*4);
}

void AddTextCentered(ImDrawList* DrawList, ImVec2 top_center, ImU32 col, const char* text_begin, const char* text_end) {
    float txt_ht = ImGui::GetTextLineHeight();
    const char* title_end = ImGui::FindRenderedTextEnd(text_begin, text_end);
    ImVec2 text_size;
    float  y = 0;
    while (const char* tmp = (const char*)memchr(text_begin, '\n', title_end-text_begin)) {
        text_size = ImGui::CalcTextSize(text_begin,tmp,true);
        DrawList->AddText(ImVec2(top_center.x - text_size.x * 0.5f, top_center.y+y),col,text_begin,tmp);
        text_begin = tmp + 1;
        y += txt_ht;
    }
    text_size = ImGui::CalcTextSize(text_begin,title_end,true);
    DrawList->AddText(ImVec2(top_center.x - text_size.x * 0.5f, top_center.y+y),col,text_begin,title_end);
}

double NiceNum(double x, bool round) {
    double f;
    double nf;
    int expv = (int)floor(ImLog10(x));
    f = x / ImPow(10.0, (double)expv);
    if (round)
        if (f < 1.5)
            nf = 1;
        else if (f < 3)
            nf = 2;
        else if (f < 7)
            nf = 5;
        else
            nf = 10;
    else if (f <= 1)
        nf = 1;
    else if (f <= 2)
        nf = 2;
    else if (f <= 5)
        nf = 5;
    else
        nf = 10;
    return nf * ImPow(10.0, expv);
}

//-----------------------------------------------------------------------------
// Context Utils
//-----------------------------------------------------------------------------

void SetImGuiContext(ImGuiContext* ctx) {
    ImGui::SetCurrentContext(ctx);
}

ImPlotContext* CreateContext() {
    ImPlotContext* ctx = IM_NEW(ImPlotContext)();
    Initialize(ctx);
    if (GImPlot == nullptr)
        SetCurrentContext(ctx);
    return ctx;
}

void DestroyContext(ImPlotContext* ctx) {
    if (ctx == nullptr)
        ctx = GImPlot;
    if (GImPlot == ctx)
        SetCurrentContext(nullptr);
    IM_DELETE(ctx);
}

ImPlotContext* GetCurrentContext() {
    return GImPlot;
}

void SetCurrentContext(ImPlotContext* ctx) {
    GImPlot = ctx;
}

#define IMPLOT_APPEND_CMAP(name, qual) ctx->ColormapData.Append(#name, name, sizeof(name)/sizeof(ImU32), qual)
#define IM_RGB(r,g,b) IM_COL32(r,g,b,255)

void Initialize(ImPlotContext* ctx) {
    ResetCtxForNextPlot(ctx);
    ResetCtxForNextAlignedPlots(ctx);
    ResetCtxForNextSubplot(ctx);

    const ImU32 Deep[]     = {4289753676, 4283598045, 4285048917, 4283584196, 4289950337, 4284512403, 4291005402, 4287401100, 4285839820, 4291671396                        };
    const ImU32 Dark[]     = {4280031972, 4290281015, 4283084621, 4288892568, 4278222847, 4281597951, 4280833702, 4290740727, 4288256409                                    };
    const ImU32 Pastel[]   = {4289639675, 4293119411, 4291161036, 4293184478, 4289124862, 4291624959, 4290631909, 4293712637, 4294111986                                    };
    const ImU32 Paired[]   = {4293119554, 4290017311, 4287291314, 4281114675, 4288256763, 4280031971, 4285513725, 4278222847, 4292260554, 4288298346, 4288282623, 4280834481};
    const ImU32 Viridis[]  = {4283695428, 4285867080, 4287054913, 4287455029, 4287526954, 4287402273, 4286883874, 4285579076, 4283552122, 4280737725, 4280674301            };
    const ImU32 Plasma[]   = {4287039501, 4288480321, 4289200234, 4288941455, 4287638193, 4286072780, 4284638433, 4283139314, 4281771772, 4280667900, 4280416752            };
    const ImU32 Hot[]      = {4278190144, 4278190208, 4278190271, 4278190335, 4278206719, 4278223103, 4278239231, 4278255615, 4283826175, 4289396735, 4294967295            };
    const ImU32 Cool[]     = {4294967040, 4294960666, 4294954035, 4294947661, 4294941030, 4294934656, 4294928025, 4294921651, 4294915020, 4294908646, 4294902015            };
    const ImU32 Pink[]     = {4278190154, 4282532475, 4284308894, 4285690554, 4286879686, 4287870160, 4288794330, 4289651940, 4291685869, 4293392118, 4294967295            };
    const ImU32 Jet[]      = {4289331200, 4294901760, 4294923520, 4294945280, 4294967040, 4289396565, 4283826090, 4278255615, 4278233855, 4278212095, 4278190335            };
    const ImU32 Twilight[] = {IM_RGB(226,217,226),IM_RGB(166,191,202),IM_RGB(109,144,192),IM_RGB(95,88,176),IM_RGB(83,30,124),IM_RGB(47,20,54),IM_RGB(100,25,75),IM_RGB(159,60,80),IM_RGB(192,117,94),IM_RGB(208,179,158),IM_RGB(226,217,226)};
    const ImU32 RdBu[]     = {IM_RGB(103,0,31),IM_RGB(178,24,43),IM_RGB(214,96,77),IM_RGB(244,165,130),IM_RGB(253,219,199),IM_RGB(247,247,247),IM_RGB(209,229,240),IM_RGB(146,197,222),IM_RGB(67,147,195),IM_RGB(33,102,172),IM_RGB(5,48,97)};
    const ImU32 BrBG[]     = {IM_RGB(84,48,5),IM_RGB(140,81,10),IM_RGB(191,129,45),IM_RGB(223,194,125),IM_RGB(246,232,195),IM_RGB(245,245,245),IM_RGB(199,234,229),IM_RGB(128,205,193),IM_RGB(53,151,143),IM_RGB(1,102,94),IM_RGB(0,60,48)};
    const ImU32 PiYG[]     = {IM_RGB(142,1,82),IM_RGB(197,27,125),IM_RGB(222,119,174),IM_RGB(241,182,218),IM_RGB(253,224,239),IM_RGB(247,247,247),IM_RGB(230,245,208),IM_RGB(184,225,134),IM_RGB(127,188,65),IM_RGB(77,146,33),IM_RGB(39,100,25)};
    const ImU32 Spectral[] = {IM_RGB(158,1,66),IM_RGB(213,62,79),IM_RGB(244,109,67),IM_RGB(253,174,97),IM_RGB(254,224,139),IM_RGB(255,255,191),IM_RGB(230,245,152),IM_RGB(171,221,164),IM_RGB(102,194,165),IM_RGB(50,136,189),IM_RGB(94,79,162)};
    const ImU32 Greys[]    = {IM_COL32_WHITE, IM_COL32_BLACK                                                                                                                };

    IMPLOT_APPEND_CMAP(Deep, true);
    IMPLOT_APPEND_CMAP(Dark, true);
    IMPLOT_APPEND_CMAP(Pastel, true);
    IMPLOT_APPEND_CMAP(Paired, true);
    IMPLOT_APPEND_CMAP(Viridis, false);
    IMPLOT_APPEND_CMAP(Plasma, false);
    IMPLOT_APPEND_CMAP(Hot, false);
    IMPLOT_APPEND_CMAP(Cool, false);
    IMPLOT_APPEND_CMAP(Pink, false);
    IMPLOT_APPEND_CMAP(Jet, false);
    IMPLOT_APPEND_CMAP(Twilight, false);
    IMPLOT_APPEND_CMAP(RdBu, false);
    IMPLOT_APPEND_CMAP(BrBG, false);
    IMPLOT_APPEND_CMAP(PiYG, false);
    IMPLOT_APPEND_CMAP(Spectral, false);
    IMPLOT_APPEND_CMAP(Greys, false);
}

void ResetCtxForNextPlot(ImPlotContext* ctx) {
    // reset the next plot/item data
    ctx->NextPlotData.Reset();
    ctx->NextItemData.Reset();
    // reset labels
    ctx->Annotations.Reset();
    ctx->Tags.Reset();
    // reset extents/fit
    ctx->OpenContextThisFrame = false;
    // reset digital plot items count
    ctx->DigitalPlotItemCnt = 0;
    ctx->DigitalPlotOffset = 0;
    // nullify plot
    ctx->CurrentPlot  = nullptr;
    ctx->CurrentItem  = nullptr;
    ctx->PreviousItem = nullptr;
}

void ResetCtxForNextAlignedPlots(ImPlotContext* ctx) {
    ctx->CurrentAlignmentH = nullptr;
    ctx->CurrentAlignmentV = nullptr;
}

void ResetCtxForNextSubplot(ImPlotContext* ctx) {
    ctx->CurrentSubplot      = nullptr;
    ctx->CurrentAlignmentH   = nullptr;
    ctx->CurrentAlignmentV   = nullptr;
}

//-----------------------------------------------------------------------------
// Plot Utils
//-----------------------------------------------------------------------------

ImPlotPlot* GetPlot(const char* title) {
    ImGuiWindow*   Window = GImGui->CurrentWindow;
    const ImGuiID  ID     = Window->GetID(title);
    return GImPlot->Plots.GetByKey(ID);
}

ImPlotPlot* GetCurrentPlot() {
    return GImPlot->CurrentPlot;
}

void BustPlotCache() {
    ImPlotContext& gp = *GImPlot;
    gp.Plots.Clear();
    gp.Subplots.Clear();
}

//-----------------------------------------------------------------------------
// Legend Utils
//-----------------------------------------------------------------------------

ImVec2 GetLocationPos(const ImRect& outer_rect, const ImVec2& inner_size, ImPlotLocation loc, const ImVec2& pad) {
    ImVec2 pos;
    if (ImHasFlag(loc, ImPlotLocation_West) && !ImHasFlag(loc, ImPlotLocation_East))
        pos.x = outer_rect.Min.x + pad.x;
    else if (!ImHasFlag(loc, ImPlotLocation_West) && ImHasFlag(loc, ImPlotLocation_East))
        pos.x = outer_rect.Max.x - pad.x - inner_size.x;
    else
        pos.x = outer_rect.GetCenter().x - inner_size.x * 0.5f;
    // legend reference point y
    if (ImHasFlag(loc, ImPlotLocation_North) && !ImHasFlag(loc, ImPlotLocation_South))
        pos.y = outer_rect.Min.y + pad.y;
    else if (!ImHasFlag(loc, ImPlotLocation_North) && ImHasFlag(loc, ImPlotLocation_South))
        pos.y = outer_rect.Max.y - pad.y - inner_size.y;
    else
        pos.y = outer_rect.GetCenter().y - inner_size.y * 0.5f;
    pos.x = IM_ROUND(pos.x);
    pos.y = IM_ROUND(pos.y);
    return pos;
}

ImVec2 CalcLegendSize(ImPlotItemGroup& items, const ImVec2& pad, const ImVec2& spacing, bool vertical) {
    // vars
    const int   nItems      = items.GetLegendCount();
    const float txt_ht      = ImGui::GetTextLineHeight();
    const float icon_size   = txt_ht;
    // get label max width
    float max_label_width = 0;
    float sum_label_width = 0;
    for (int i = 0; i < nItems; ++i) {
        const char* label       = items.GetLegendLabel(i);
        const float label_width = ImGui::CalcTextSize(label, nullptr, true).x;
        max_label_width         = label_width > max_label_width ? label_width : max_label_width;
        sum_label_width        += label_width;
    }
    // calc legend size
    const ImVec2 legend_size = vertical ?
                               ImVec2(pad.x * 2 + icon_size + max_label_width, pad.y * 2 + nItems * txt_ht + (nItems - 1) * spacing.y) :
                               ImVec2(pad.x * 2 + icon_size * nItems + sum_label_width + (nItems - 1) * spacing.x, pad.y * 2 + txt_ht);
    return legend_size;
}

bool ClampLegendRect(ImRect& legend_rect, const ImRect& outer_rect, const ImVec2& pad) {
    bool clamped = false;
    ImRect outer_rect_pad(outer_rect.Min + pad, outer_rect.Max - pad);
    if (legend_rect.Min.x < outer_rect_pad.Min.x) {
        legend_rect.Min.x = outer_rect_pad.Min.x;
        clamped = true;
    }
    if (legend_rect.Min.y < outer_rect_pad.Min.y) {
        legend_rect.Min.y = outer_rect_pad.Min.y;
        clamped = true;
    }
    if (legend_rect.Max.x > outer_rect_pad.Max.x) {
        legend_rect.Max.x = outer_rect_pad.Max.x;
        clamped = true;
    }
    if (legend_rect.Max.y > outer_rect_pad.Max.y) {
        legend_rect.Max.y = outer_rect_pad.Max.y;
        clamped = true;
    }
    return clamped;
}

int LegendSortingComp(const void* _a, const void* _b) {
    ImPlotItemGroup* items = GImPlot->SortItems;
    const int a = *(const int*)_a;
    const int b = *(const int*)_b;
    const char* label_a = items->GetLegendLabel(a);
    const char* label_b = items->GetLegendLabel(b);
    return strcmp(label_a,label_b);
}

bool ShowLegendEntries(ImPlotItemGroup& items, const ImRect& legend_bb, bool hovered, const ImVec2& pad, const ImVec2& spacing, bool vertical, ImDrawList& DrawList) {
    // vars
    const float txt_ht      = ImGui::GetTextLineHeight();
    const float icon_size   = txt_ht;
    const float icon_shrink = 2;
    ImU32 col_txt           = GetStyleColorU32(ImPlotCol_LegendText);
    ImU32 col_txt_dis       = ImAlphaU32(col_txt, 0.25f);
    // render each legend item
    float sum_label_width = 0;
    bool any_item_hovered = false;

    const int num_items = items.GetLegendCount();
    if (num_items < 1)
        return hovered;
    // build render order
    ImPlotContext& gp = *GImPlot;
    ImVector<int>& indices = gp.TempInt1;
    indices.resize(num_items);
    for (int i = 0; i < num_items; ++i)
        indices[i] = i;
    if (ImHasFlag(items.Legend.Flags, ImPlotLegendFlags_Sort) && num_items > 1) {
        gp.SortItems = &items;
        qsort(indices.Data, num_items, sizeof(int), LegendSortingComp);
    }
    // render
    for (int i = 0; i < num_items; ++i) {
        const int idx           = indices[i];
        ImPlotItem* item        = items.GetLegendItem(idx);
        const char* label       = items.GetLegendLabel(idx);
        const float label_width = ImGui::CalcTextSize(label, nullptr, true).x;
        const ImVec2 top_left   = vertical ?
                                  legend_bb.Min + pad + ImVec2(0, i * (txt_ht + spacing.y)) :
                                  legend_bb.Min + pad + ImVec2(i * (icon_size + spacing.x) + sum_label_width, 0);
        sum_label_width        += label_width;
        ImRect icon_bb;
        icon_bb.Min = top_left + ImVec2(icon_shrink,icon_shrink);
        icon_bb.Max = top_left + ImVec2(icon_size - icon_shrink, icon_size - icon_shrink);
        ImRect label_bb;
        label_bb.Min = top_left;
        label_bb.Max = top_left + ImVec2(label_width + icon_size, icon_size);
        ImU32 col_txt_hl;
        ImU32 col_item = ImAlphaU32(item->Color,1);

        ImRect button_bb(icon_bb.Min, label_bb.Max);

        ImGui::KeepAliveID(item->ID);

        bool item_hov = false;
        bool item_hld = false;
        bool item_clk = ImHasFlag(items.Legend.Flags, ImPlotLegendFlags_NoButtons)
                      ? false
                      : ImGui::ButtonBehavior(button_bb, item->ID, &item_hov, &item_hld);

        if (item_clk)
            item->Show = !item->Show;


        const bool can_hover = (item_hov)
                             && (!ImHasFlag(items.Legend.Flags, ImPlotLegendFlags_NoHighlightItem)
                             || !ImHasFlag(items.Legend.Flags, ImPlotLegendFlags_NoHighlightAxis));

        if (can_hover) {
            item->LegendHoverRect.Min = icon_bb.Min;
            item->LegendHoverRect.Max = label_bb.Max;
            item->LegendHovered = true;
            col_txt_hl = ImMixU32(col_txt, col_item, 64);
            any_item_hovered = true;
        }
        else {
            col_txt_hl = ImGui::GetColorU32(col_txt);
        }
        ImU32 col_icon;
        if (item_hld)
            col_icon = item->Show ? ImAlphaU32(col_item,0.5f) : ImGui::GetColorU32(ImGuiCol_TextDisabled, 0.5f);
        else if (item_hov)
            col_icon = item->Show ? ImAlphaU32(col_item,0.75f) : ImGui::GetColorU32(ImGuiCol_TextDisabled, 0.75f);
        else
            col_icon = item->Show ? col_item : col_txt_dis;

        DrawList.AddRectFilled(icon_bb.Min, icon_bb.Max, col_icon);
        const char* text_display_end = ImGui::FindRenderedTextEnd(label, nullptr);
        if (label != text_display_end)
            DrawList.AddText(top_left + ImVec2(icon_size, 0), item->Show ? col_txt_hl  : col_txt_dis, label, text_display_end);
    }
    return hovered && !any_item_hovered;
}

//-----------------------------------------------------------------------------
// Locators
//-----------------------------------------------------------------------------

static const float TICK_FILL_X = 0.8f;
static const float TICK_FILL_Y = 1.0f;

void Locator_Default(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data) {
    if (range.Min == range.Max)
        return;
    const int nMinor        = 10;
    const int nMajor        = ImMax(2, (int)IM_ROUND(pixels / (vertical ? 300.0f : 400.0f)));
    const double nice_range = NiceNum(range.Size() * 0.99, false);
    const double interval   = NiceNum(nice_range / (nMajor - 1), true);
    const double graphmin   = floor(range.Min / interval) * interval;
    const double graphmax   = ceil(range.Max / interval) * interval;
    bool first_major_set    = false;
    int  first_major_idx    = 0;
    const int idx0 = ticker.TickCount(); // ticker may have user custom ticks
    ImVec2 total_size(0,0);
    for (double major = graphmin; major < graphmax + 0.5 * interval; major += interval) {
        // is this zero? combat zero formatting issues
        if (major-interval < 0 && major+interval > 0)
            major = 0;
        if (range.Contains(major)) {
            if (!first_major_set) {
                first_major_idx = ticker.TickCount();
                first_major_set = true;
            }
            total_size += ticker.AddTick(major, true, 0, true, formatter, formatter_data).LabelSize;
        }
        for (int i = 1; i < nMinor; ++i) {
            double minor = major + i * interval / nMinor;
            if (range.Contains(minor)) {
                total_size += ticker.AddTick(minor, false, 0, true, formatter, formatter_data).LabelSize;
            }
        }
    }
    // prune if necessary
    if ((!vertical && total_size.x > pixels*TICK_FILL_X) || (vertical && total_size.y > pixels*TICK_FILL_Y)) {
        for (int i = first_major_idx-1; i >= idx0; i -= 2)
            ticker.Ticks[i].ShowLabel = false;
        for (int i = first_major_idx+1; i < ticker.TickCount(); i += 2)
            ticker.Ticks[i].ShowLabel = false;
    }
}

bool CalcLogarithmicExponents(const ImPlotRange& range, float pix, bool vertical, int& exp_min, int& exp_max, int& exp_step) {
    if (range.Min * range.Max > 0) {
        const int nMajor = vertical ? ImMax(2, (int)IM_ROUND(pix * 0.02f)) : ImMax(2, (int)IM_ROUND(pix * 0.01f)); // TODO: magic numbers
        double log_min = ImLog10(ImAbs(range.Min));
        double log_max = ImLog10(ImAbs(range.Max));
        double log_a = ImMin(log_min,log_max);
        double log_b = ImMax(log_min,log_max);
        exp_step  = ImMax(1,(int)(log_b - log_a) / nMajor);
        exp_min   = (int)log_a;
        exp_max   = (int)log_b;
        if (exp_step != 1) {
            while(exp_step % 3 != 0)       exp_step++; // make step size multiple of three
            while(exp_min % exp_step != 0) exp_min--;  // decrease exp_min until exp_min + N * exp_step will be 0
        }
        return true;
    }
    return false;
}

void AddTicksLogarithmic(const ImPlotRange& range, int exp_min, int exp_max, int exp_step, ImPlotTicker& ticker, ImPlotFormatter formatter, void* data) {
    const double sign = ImSign(range.Max);
    for (int e = exp_min - exp_step; e < (exp_max + exp_step); e += exp_step) {
        double major1 = sign*ImPow(10, (double)(e));
        double major2 = sign*ImPow(10, (double)(e + 1));
        double interval = (major2 - major1) / 9;
        if (major1 >= (range.Min - DBL_EPSILON) && major1 <= (range.Max + DBL_EPSILON))
            ticker.AddTick(major1, true, 0, true, formatter, data);
        for (int j = 0; j < exp_step; ++j) {
            major1 = sign*ImPow(10, (double)(e+j));
            major2 = sign*ImPow(10, (double)(e+j+1));
            interval = (major2 - major1) / 9;
            for (int i = 1; i < (9 + (int)(j < (exp_step - 1))); ++i) {
                double minor = major1 + i * interval;
                if (minor >= (range.Min - DBL_EPSILON) && minor <= (range.Max + DBL_EPSILON))
                    ticker.AddTick(minor, false, 0, false, formatter, data);
            }
        }
    }
}

void Locator_Log10(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data) {
    int exp_min, exp_max, exp_step;
    if (CalcLogarithmicExponents(range, pixels, vertical, exp_min, exp_max, exp_step))
        AddTicksLogarithmic(range, exp_min, exp_max, exp_step, ticker, formatter, formatter_data);
}

float CalcSymLogPixel(double plt, const ImPlotRange& range, float pixels) {
    double scaleToPixels = pixels / range.Size();
    double scaleMin      = TransformForward_SymLog(range.Min,nullptr);
    double scaleMax      = TransformForward_SymLog(range.Max,nullptr);
    double s             = TransformForward_SymLog(plt, nullptr);
    double t             = (s - scaleMin) / (scaleMax - scaleMin);
    plt                  = range.Min + range.Size() * t;

    return (float)(0 + scaleToPixels * (plt - range.Min));
}

void Locator_SymLog(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data) {
    if (range.Min >= -1 && range.Max <= 1) {
        Locator_Default(ticker, range, pixels, vertical, formatter, formatter_data);
    }
    else if (range.Min * range.Max < 0) { // cross zero
        const float pix_min = 0;
        const float pix_max = pixels;
        const float pix_p1  = CalcSymLogPixel(1, range, pixels);
        const float pix_n1  = CalcSymLogPixel(-1, range, pixels);
        int exp_min_p, exp_max_p, exp_step_p;
        int exp_min_n, exp_max_n, exp_step_n;
        CalcLogarithmicExponents(ImPlotRange(1,range.Max), ImAbs(pix_max-pix_p1),vertical,exp_min_p,exp_max_p,exp_step_p);
        CalcLogarithmicExponents(ImPlotRange(range.Min,-1),ImAbs(pix_n1-pix_min),vertical,exp_min_n,exp_max_n,exp_step_n);
        int exp_step = ImMax(exp_step_n, exp_step_p);
        ticker.AddTick(0,true,0,true,formatter,formatter_data);
        AddTicksLogarithmic(ImPlotRange(1,range.Max), exp_min_p,exp_max_p,exp_step,ticker,formatter,formatter_data);
        AddTicksLogarithmic(ImPlotRange(range.Min,-1),exp_min_n,exp_max_n,exp_step,ticker,formatter,formatter_data);
    }
    else {
        Locator_Log10(ticker, range, pixels, vertical, formatter, formatter_data);
    }
}

void AddTicksCustom(const double* values, const char* const labels[], int n, ImPlotTicker& ticker, ImPlotFormatter formatter, void* data) {
    for (int i = 0; i < n; ++i) {
        if (labels != nullptr)
            ticker.AddTick(values[i], false, 0, true, labels[i]);
        else
            ticker.AddTick(values[i], false, 0, true, formatter, data);
    }
}

//-----------------------------------------------------------------------------
// Time Ticks and Utils
//-----------------------------------------------------------------------------

// this may not be thread safe?
static const double TimeUnitSpans[ImPlotTimeUnit_COUNT] = {
    0.000001,
    0.001,
    1,
    60,
    3600,
    86400,
    2629800,
    31557600
};

inline ImPlotTimeUnit GetUnitForRange(double range) {
    static double cutoffs[ImPlotTimeUnit_COUNT] = {0.001, 1, 60, 3600, 86400, 2629800, 31557600, IMPLOT_MAX_TIME};
    for (int i = 0; i < ImPlotTimeUnit_COUNT; ++i) {
        if (range <= cutoffs[i])
            return (ImPlotTimeUnit)i;
    }
    return ImPlotTimeUnit_Yr;
}

inline int LowerBoundStep(int max_divs, const int* divs, const int* step, int size) {
    if (max_divs < divs[0])
        return 0;
    for (int i = 1; i < size; ++i) {
        if (max_divs < divs[i])
            return step[i-1];
    }
    return step[size-1];
}

inline int GetTimeStep(int max_divs, ImPlotTimeUnit unit) {
    if (unit == ImPlotTimeUnit_Ms || unit == ImPlotTimeUnit_Us) {
        static const int step[] = {500,250,200,100,50,25,20,10,5,2,1};
        static const int divs[] = {2,4,5,10,20,40,50,100,200,500,1000};
        return LowerBoundStep(max_divs, divs, step, 11);
    }
    if (unit == ImPlotTimeUnit_S || unit == ImPlotTimeUnit_Min) {
        static const int step[] = {30,15,10,5,1};
        static const int divs[] = {2,4,6,12,60};
        return LowerBoundStep(max_divs, divs, step, 5);
    }
    else if (unit == ImPlotTimeUnit_Hr) {
        static const int step[] = {12,6,3,2,1};
        static const int divs[] = {2,4,8,12,24};
        return LowerBoundStep(max_divs, divs, step, 5);
    }
    else if (unit == ImPlotTimeUnit_Day) {
        static const int step[] = {14,7,2,1};
        static const int divs[] = {2,4,14,28};
        return LowerBoundStep(max_divs, divs, step, 4);
    }
    else if (unit == ImPlotTimeUnit_Mo) {
        static const int step[] = {6,3,2,1};
        static const int divs[] = {2,4,6,12};
        return LowerBoundStep(max_divs, divs, step, 4);
    }
    return 0;
}

ImPlotTime MkGmtTime(struct tm *ptm) {
    ImPlotTime t;
#ifdef _WIN32
    t.S = _mkgmtime(ptm);
#else
    t.S = timegm(ptm);
#endif
    if (t.S < 0)
        t.S = 0;
    return t;
}

tm* GetGmtTime(const ImPlotTime& t, tm* ptm)
{
#ifdef _WIN32
  if (gmtime_s(ptm, &t.S) == 0)
    return ptm;
  else
    return nullptr;
#else
  return gmtime_r(&t.S, ptm);
#endif
}

ImPlotTime MkLocTime(struct tm *ptm) {
    ImPlotTime t;
    t.S = mktime(ptm);
    if (t.S < 0)
        t.S = 0;
    return t;
}

tm* GetLocTime(const ImPlotTime& t, tm* ptm) {
#ifdef _WIN32
  if (localtime_s(ptm, &t.S) == 0)
    return ptm;
  else
    return nullptr;
#else
    return localtime_r(&t.S, ptm);
#endif
}

inline ImPlotTime MkTime(struct tm *ptm) {
    if (GetStyle().UseLocalTime)
        return MkLocTime(ptm);
    else
        return MkGmtTime(ptm);
}

inline tm* GetTime(const ImPlotTime& t, tm* ptm) {
    if (GetStyle().UseLocalTime)
        return GetLocTime(t,ptm);
    else
        return GetGmtTime(t,ptm);
}

ImPlotTime MakeTime(int year, int month, int day, int hour, int min, int sec, int us) {
    tm& Tm = GImPlot->Tm;

    int yr = year - 1900;
    if (yr < 0)
        yr = 0;

    sec  = sec + us / 1000000;
    us   = us % 1000000;

    Tm.tm_sec  = sec;
    Tm.tm_min  = min;
    Tm.tm_hour = hour;
    Tm.tm_mday = day;
    Tm.tm_mon  = month;
    Tm.tm_year = yr;

    ImPlotTime t = MkTime(&Tm);

    t.Us = us;
    return t;
}

int GetYear(const ImPlotTime& t) {
    tm& Tm = GImPlot->Tm;
    GetTime(t, &Tm);
    return Tm.tm_year + 1900;
}

ImPlotTime AddTime(const ImPlotTime& t, ImPlotTimeUnit unit, int count) {
    tm& Tm = GImPlot->Tm;
    ImPlotTime t_out = t;
    switch(unit) {
        case ImPlotTimeUnit_Us:  t_out.Us += count;         break;
        case ImPlotTimeUnit_Ms:  t_out.Us += count * 1000;  break;
        case ImPlotTimeUnit_S:   t_out.S  += count;         break;
        case ImPlotTimeUnit_Min: t_out.S  += count * 60;    break;
        case ImPlotTimeUnit_Hr:  t_out.S  += count * 3600;  break;
        case ImPlotTimeUnit_Day: t_out.S  += count * 86400; break;
        case ImPlotTimeUnit_Mo:  for (int i = 0; i < abs(count); ++i) {
                                     GetTime(t_out, &Tm);
                                     if (count > 0)
                                        t_out.S += 86400 * GetDaysInMonth(Tm.tm_year + 1900, Tm.tm_mon);
                                     else if (count < 0)
                                        t_out.S -= 86400 * GetDaysInMonth(Tm.tm_year + 1900 - (Tm.tm_mon == 0 ? 1 : 0), Tm.tm_mon == 0 ? 11 : Tm.tm_mon - 1); // NOT WORKING
                                 }
                                 break;
        case ImPlotTimeUnit_Yr:  for (int i = 0; i < abs(count); ++i) {
                                    if (count > 0)
                                        t_out.S += 86400 * (365 + (int)IsLeapYear(GetYear(t_out)));
                                    else if (count < 0)
                                        t_out.S -= 86400 * (365 + (int)IsLeapYear(GetYear(t_out) - 1));
                                    // this is incorrect if leap year and we are past Feb 28
                                 }
                                 break;
        default:                 break;
    }
    t_out.RollOver();
    return t_out;
}

ImPlotTime FloorTime(const ImPlotTime& t, ImPlotTimeUnit unit) {
    ImPlotContext& gp = *GImPlot;
    GetTime(t, &gp.Tm);
    switch (unit) {
        case ImPlotTimeUnit_S:   return ImPlotTime(t.S, 0);
        case ImPlotTimeUnit_Ms:  return ImPlotTime(t.S, (t.Us / 1000) * 1000);
        case ImPlotTimeUnit_Us:  return t;
        case ImPlotTimeUnit_Yr:  gp.Tm.tm_mon  = 0; // fall-through
        case ImPlotTimeUnit_Mo:  gp.Tm.tm_mday = 1; // fall-through
        case ImPlotTimeUnit_Day: gp.Tm.tm_hour = 0; // fall-through
        case ImPlotTimeUnit_Hr:  gp.Tm.tm_min  = 0; // fall-through
        case ImPlotTimeUnit_Min: gp.Tm.tm_sec  = 0; break;
        default:                 return t;
    }
    return MkTime(&gp.Tm);
}

ImPlotTime CeilTime(const ImPlotTime& t, ImPlotTimeUnit unit) {
    return AddTime(FloorTime(t, unit), unit, 1);
}

ImPlotTime RoundTime(const ImPlotTime& t, ImPlotTimeUnit unit) {
    ImPlotTime t1 = FloorTime(t, unit);
    ImPlotTime t2 = AddTime(t1,unit,1);
    if (t1.S == t2.S)
        return t.Us - t1.Us < t2.Us - t.Us ? t1 : t2;
    return t.S - t1.S < t2.S - t.S ? t1 : t2;
}

ImPlotTime CombineDateTime(const ImPlotTime& date_part, const ImPlotTime& tod_part) {
    ImPlotContext& gp = *GImPlot;
    tm& Tm = gp.Tm;
    GetTime(date_part, &gp.Tm);
    int y = Tm.tm_year;
    int m = Tm.tm_mon;
    int d = Tm.tm_mday;
    GetTime(tod_part, &gp.Tm);
    Tm.tm_year = y;
    Tm.tm_mon  = m;
    Tm.tm_mday = d;
    ImPlotTime t = MkTime(&Tm);
    t.Us = tod_part.Us;
    return t;
}

// TODO: allow users to define these
static const char* MONTH_NAMES[]  = {"January","February","March","April","May","June","July","August","September","October","November","December"};
static const char* WD_ABRVS[]     = {"Su","Mo","Tu","We","Th","Fr","Sa"};
static const char* MONTH_ABRVS[]  = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

int FormatTime(const ImPlotTime& t, char* buffer, int size, ImPlotTimeFmt fmt, bool use_24_hr_clk) {
    tm& Tm = GImPlot->Tm;
    GetTime(t, &Tm);
    const int us   = t.Us % 1000;
    const int ms   = t.Us / 1000;
    const int sec  = Tm.tm_sec;
    const int min  = Tm.tm_min;
    if (use_24_hr_clk) {
        const int hr   = Tm.tm_hour;
        switch(fmt) {
            case ImPlotTimeFmt_Us:        return ImFormatString(buffer, size, ".%03d %03d", ms, us);
            case ImPlotTimeFmt_SUs:       return ImFormatString(buffer, size, ":%02d.%03d %03d", sec, ms, us);
            case ImPlotTimeFmt_SMs:       return ImFormatString(buffer, size, ":%02d.%03d", sec, ms);
            case ImPlotTimeFmt_S:         return ImFormatString(buffer, size, ":%02d", sec);
            case ImPlotTimeFmt_MinSMs:    return ImFormatString(buffer, size, ":%02d:%02d.%03d", min, sec, ms);
            case ImPlotTimeFmt_HrMinSMs:  return ImFormatString(buffer, size, "%02d:%02d:%02d.%03d", hr, min, sec, ms);
            case ImPlotTimeFmt_HrMinS:    return ImFormatString(buffer, size, "%02d:%02d:%02d", hr, min, sec);
            case ImPlotTimeFmt_HrMin:     return ImFormatString(buffer, size, "%02d:%02d", hr, min);
            case ImPlotTimeFmt_Hr:        return ImFormatString(buffer, size, "%02d:00", hr);
            default:                      return 0;
        }
    }
    else {
        const char* ap = Tm.tm_hour < 12 ? "am" : "pm";
        const int hr   = (Tm.tm_hour == 0 || Tm.tm_hour == 12) ? 12 : Tm.tm_hour % 12;
        switch(fmt) {
            case ImPlotTimeFmt_Us:        return ImFormatString(buffer, size, ".%03d %03d", ms, us);
            case ImPlotTimeFmt_SUs:       return ImFormatString(buffer, size, ":%02d.%03d %03d", sec, ms, us);
            case ImPlotTimeFmt_SMs:       return ImFormatString(buffer, size, ":%02d.%03d", sec, ms);
            case ImPlotTimeFmt_S:         return ImFormatString(buffer, size, ":%02d", sec);
            case ImPlotTimeFmt_MinSMs:    return ImFormatString(buffer, size, ":%02d:%02d.%03d", min, sec, ms);
            case ImPlotTimeFmt_HrMinSMs:  return ImFormatString(buffer, size, "%d:%02d:%02d.%03d%s", hr, min, sec, ms, ap);
            case ImPlotTimeFmt_HrMinS:    return ImFormatString(buffer, size, "%d:%02d:%02d%s", hr, min, sec, ap);
            case ImPlotTimeFmt_HrMin:     return ImFormatString(buffer, size, "%d:%02d%s", hr, min, ap);
            case ImPlotTimeFmt_Hr:        return ImFormatString(buffer, size, "%d%s", hr, ap);
            default:                      return 0;
        }
    }
}

int FormatDate(const ImPlotTime& t, char* buffer, int size, ImPlotDateFmt fmt, bool use_iso_8601) {
    tm& Tm = GImPlot->Tm;
    GetTime(t, &Tm);
    const int day  = Tm.tm_mday;
    const int mon  = Tm.tm_mon + 1;
    const int year = Tm.tm_year + 1900;
    const int yr   = year % 100;
    if (use_iso_8601) {
        switch (fmt) {
            case ImPlotDateFmt_DayMo:   return ImFormatString(buffer, size, "--%02d-%02d", mon, day);
            case ImPlotDateFmt_DayMoYr: return ImFormatString(buffer, size, "%d-%02d-%02d", year, mon, day);
            case ImPlotDateFmt_MoYr:    return ImFormatString(buffer, size, "%d-%02d", year, mon);
            case ImPlotDateFmt_Mo:      return ImFormatString(buffer, size, "--%02d", mon);
            case ImPlotDateFmt_Yr:      return ImFormatString(buffer, size, "%d", year);
            default:                    return 0;
        }
    }
    else {
        switch (fmt) {
            case ImPlotDateFmt_DayMo:   return ImFormatString(buffer, size, "%d/%d", mon, day);
            case ImPlotDateFmt_DayMoYr: return ImFormatString(buffer, size, "%d/%d/%02d", mon, day, yr);
            case ImPlotDateFmt_MoYr:    return ImFormatString(buffer, size, "%s %d", MONTH_ABRVS[Tm.tm_mon], year);
            case ImPlotDateFmt_Mo:      return ImFormatString(buffer, size, "%s", MONTH_ABRVS[Tm.tm_mon]);
            case ImPlotDateFmt_Yr:      return ImFormatString(buffer, size, "%d", year);
            default:                    return 0;
        }
    }
 }

int FormatDateTime(const ImPlotTime& t, char* buffer, int size, ImPlotDateTimeSpec fmt) {
    int written = 0;
    if (fmt.Date != ImPlotDateFmt_None)
        written += FormatDate(t, buffer, size, fmt.Date, fmt.UseISO8601);
    if (fmt.Time != ImPlotTimeFmt_None) {
        if (fmt.Date != ImPlotDateFmt_None)
            buffer[written++] = ' ';
        written += FormatTime(t, &buffer[written], size - written, fmt.Time, fmt.Use24HourClock);
    }
    return written;
}

inline float GetDateTimeWidth(ImPlotDateTimeSpec fmt) {
    static const ImPlotTime t_max_width = MakeTime(2888, 12, 22, 12, 58, 58, 888888); // best guess at time that maximizes pixel width
    char buffer[32];
    FormatDateTime(t_max_width, buffer, 32, fmt);
    return ImGui::CalcTextSize(buffer).x;
}

inline bool TimeLabelSame(const char* l1, const char* l2) {
    size_t len1 = strlen(l1);
    size_t len2 = strlen(l2);
    size_t n  = len1 < len2 ? len1 : len2;
    return strcmp(l1 + len1 - n, l2 + len2 - n) == 0;
}

static const ImPlotDateTimeSpec TimeFormatLevel0[ImPlotTimeUnit_COUNT] = {
    ImPlotDateTimeSpec(ImPlotDateFmt_None,  ImPlotTimeFmt_Us),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,  ImPlotTimeFmt_SMs),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,  ImPlotTimeFmt_S),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,  ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,  ImPlotTimeFmt_Hr),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMo, ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_Mo,    ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_Yr,    ImPlotTimeFmt_None)
};

static const ImPlotDateTimeSpec TimeFormatLevel1[ImPlotTimeUnit_COUNT] = {
    ImPlotDateTimeSpec(ImPlotDateFmt_None,    ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,    ImPlotTimeFmt_HrMinS),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,    ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,    ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_Yr,      ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_Yr,      ImPlotTimeFmt_None)
};

static const ImPlotDateTimeSpec TimeFormatLevel1First[ImPlotTimeUnit_COUNT] = {
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMinS),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMinS),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_Yr,      ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_Yr,      ImPlotTimeFmt_None)
};

static const ImPlotDateTimeSpec TimeFormatMouseCursor[ImPlotTimeUnit_COUNT] = {
    ImPlotDateTimeSpec(ImPlotDateFmt_None,     ImPlotTimeFmt_Us),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,     ImPlotTimeFmt_SUs),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,     ImPlotTimeFmt_SMs),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,     ImPlotTimeFmt_HrMinS),
    ImPlotDateTimeSpec(ImPlotDateFmt_None,     ImPlotTimeFmt_HrMin),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMo,    ImPlotTimeFmt_Hr),
    ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr,  ImPlotTimeFmt_None),
    ImPlotDateTimeSpec(ImPlotDateFmt_MoYr,     ImPlotTimeFmt_None)
};

inline ImPlotDateTimeSpec GetDateTimeFmt(const ImPlotDateTimeSpec* ctx, ImPlotTimeUnit idx) {
    ImPlotStyle& style     = GetStyle();
    ImPlotDateTimeSpec fmt  = ctx[idx];
    fmt.UseISO8601         = style.UseISO8601;
    fmt.Use24HourClock     = style.Use24HourClock;
    return fmt;
}

void Locator_Time(ImPlotTicker& ticker, const ImPlotRange& range, float pixels, bool vertical, ImPlotFormatter formatter, void* formatter_data) {
    IM_ASSERT_USER_ERROR(vertical == false, "Cannot locate Time ticks on vertical axis!");
    (void)vertical;
    // get units for level 0 and level 1 labels
    const ImPlotTimeUnit unit0 = GetUnitForRange(range.Size() / (pixels / 100)); // level = 0 (top)
    const ImPlotTimeUnit unit1 = ImClamp(unit0 + 1, 0, ImPlotTimeUnit_COUNT-1);  // level = 1 (bottom)
    // get time format specs
    const ImPlotDateTimeSpec fmt0 = GetDateTimeFmt(TimeFormatLevel0, unit0);
    const ImPlotDateTimeSpec fmt1 = GetDateTimeFmt(TimeFormatLevel1, unit1);
    const ImPlotDateTimeSpec fmtf = GetDateTimeFmt(TimeFormatLevel1First, unit1);
    // min max times
    const ImPlotTime t_min = ImPlotTime::FromDouble(range.Min);
    const ImPlotTime t_max = ImPlotTime::FromDouble(range.Max);
    // maximum allowable density of labels
    const float max_density = 0.5f;
    // book keeping
    int last_major_offset = -1;
    // formatter data
    Formatter_Time_Data ftd;
    ftd.UserFormatter = formatter;
    ftd.UserFormatterData = formatter_data;
    if (unit0 != ImPlotTimeUnit_Yr) {
        // pixels per major (level 1) division
        const float pix_per_major_div = pixels / (float)(range.Size() / TimeUnitSpans[unit1]);
        // nominal pixels taken up by labels
        const float fmt0_width = GetDateTimeWidth(fmt0);
        const float fmt1_width = GetDateTimeWidth(fmt1);
        const float fmtf_width = GetDateTimeWidth(fmtf);
        // the maximum number of minor (level 0) labels that can fit between major (level 1) divisions
        const int   minor_per_major   = (int)(max_density * pix_per_major_div / fmt0_width);
        // the minor step size (level 0)
        const int step = GetTimeStep(minor_per_major, unit0);
        // generate ticks
        ImPlotTime t1 = FloorTime(ImPlotTime::FromDouble(range.Min), unit1);
        while (t1 < t_max) {
            // get next major
            const ImPlotTime t2 = AddTime(t1, unit1, 1);
            // add major tick
            if (t1 >= t_min && t1 <= t_max) {
                // minor level 0 tick
                ftd.Time = t1; ftd.Spec = fmt0;
                ticker.AddTick(t1.ToDouble(), true, 0, true, Formatter_Time, &ftd);
                // major level 1 tick
                ftd.Time = t1; ftd.Spec = last_major_offset < 0 ? fmtf : fmt1;
                ImPlotTick& tick_maj = ticker.AddTick(t1.ToDouble(), true, 1, true, Formatter_Time, &ftd);
                const char* this_major = ticker.GetText(tick_maj);
                if (last_major_offset >= 0 && TimeLabelSame(ticker.TextBuffer.Buf.Data + last_major_offset, this_major))
                    tick_maj.ShowLabel = false;
                last_major_offset = tick_maj.TextOffset;
            }
            // add minor ticks up until next major
            if (minor_per_major > 1 && (t_min <= t2 && t1 <= t_max)) {
                ImPlotTime t12 = AddTime(t1, unit0, step);
                while (t12 < t2) {
                    float px_to_t2 = (float)((t2 - t12).ToDouble()/range.Size()) * pixels;
                    if (t12 >= t_min && t12 <= t_max) {
                        ftd.Time = t12; ftd.Spec = fmt0;
                        ticker.AddTick(t12.ToDouble(), false, 0, px_to_t2 >= fmt0_width, Formatter_Time, &ftd);
                        if (last_major_offset < 0 && px_to_t2 >= fmt0_width && px_to_t2 >= (fmt1_width + fmtf_width) / 2) {
                            ftd.Time = t12; ftd.Spec = fmtf;
                            ImPlotTick& tick_maj = ticker.AddTick(t12.ToDouble(), true, 1, true, Formatter_Time, &ftd);
                            last_major_offset = tick_maj.TextOffset;
                        }
                    }
                    t12 = AddTime(t12, unit0, step);
                }
            }
            t1 = t2;
        }
    }
    else {
        const ImPlotDateTimeSpec fmty = GetDateTimeFmt(TimeFormatLevel0, ImPlotTimeUnit_Yr);
        const float label_width = GetDateTimeWidth(fmty);
        const int   max_labels  = (int)(max_density * pixels / label_width);
        const int year_min      = GetYear(t_min);
        const int year_max      = GetYear(CeilTime(t_max, ImPlotTimeUnit_Yr));
        const double nice_range = NiceNum((year_max - year_min)*0.99,false);
        const double interval   = NiceNum(nice_range / (max_labels - 1), true);
        const int graphmin      = (int)(floor(year_min / interval) * interval);
        const int graphmax      = (int)(ceil(year_max  / interval) * interval);
        const int step          = (int)interval <= 0 ? 1 : (int)interval;

        for (int y = graphmin; y < graphmax; y += step) {
            ImPlotTime t = MakeTime(y);
            if (t >= t_min && t <= t_max) {
                ftd.Time = t; ftd.Spec = fmty;
                ticker.AddTick(t.ToDouble(), true, 0, true, Formatter_Time, &ftd);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Context Menu
//-----------------------------------------------------------------------------

template <typename F>
bool DragFloat(const char*, F*, float, F, F) {
    return false;
}

template <>
bool DragFloat<double>(const char* label, double* v, float v_speed, double v_min, double v_max) {
    return ImGui::DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, "%.3g", 1);
}

template <>
bool DragFloat<float>(const char* label, float* v, float v_speed, float v_min, float v_max) {
    return ImGui::DragScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, "%.3g", 1);
}

inline void BeginDisabledControls(bool cond) {
    if (cond) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.25f);
    }
}

inline void EndDisabledControls(bool cond) {
    if (cond) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

void ShowAxisContextMenu(ImPlotAxis& axis, ImPlotAxis* equal_axis, bool /*time_allowed*/) {

    ImGui::PushItemWidth(75);
    bool always_locked   = axis.IsRangeLocked() || axis.IsAutoFitting();
    bool label           = axis.HasLabel();
    bool grid            = axis.HasGridLines();
    bool ticks           = axis.HasTickMarks();
    bool labels          = axis.HasTickLabels();
    double drag_speed    = (axis.Range.Size() <= DBL_EPSILON) ? DBL_EPSILON * 1.0e+13 : 0.01 * axis.Range.Size(); // recover from almost equal axis limits.

    if (axis.Scale == ImPlotScale_Time) {
        ImPlotTime tmin = ImPlotTime::FromDouble(axis.Range.Min);
        ImPlotTime tmax = ImPlotTime::FromDouble(axis.Range.Max);

        BeginDisabledControls(always_locked);
        ImGui::CheckboxFlags("##LockMin", (unsigned int*)&axis.Flags, ImPlotAxisFlags_LockMin);
        EndDisabledControls(always_locked);
        ImGui::SameLine();
        BeginDisabledControls(axis.IsLockedMin() || always_locked);
        if (ImGui::BeginMenu("Min Time")) {
            if (ShowTimePicker("mintime", &tmin)) {
                if (tmin >= tmax)
                    tmax = AddTime(tmin, ImPlotTimeUnit_S, 1);
                axis.SetRange(tmin.ToDouble(),tmax.ToDouble());
            }
            ImGui::Separator();
            if (ShowDatePicker("mindate",&axis.PickerLevel,&axis.PickerTimeMin,&tmin,&tmax)) {
                tmin = CombineDateTime(axis.PickerTimeMin, tmin);
                if (tmin >= tmax)
                    tmax = AddTime(tmin, ImPlotTimeUnit_S, 1);
                axis.SetRange(tmin.ToDouble(), tmax.ToDouble());
            }
            ImGui::EndMenu();
        }
        EndDisabledControls(axis.IsLockedMin() || always_locked);

        BeginDisabledControls(always_locked);
        ImGui::CheckboxFlags("##LockMax", (unsigned int*)&axis.Flags, ImPlotAxisFlags_LockMax);
        EndDisabledControls(always_locked);
        ImGui::SameLine();
        BeginDisabledControls(axis.IsLockedMax() || always_locked);
        if (ImGui::BeginMenu("Max Time")) {
            if (ShowTimePicker("maxtime", &tmax)) {
                if (tmax <= tmin)
                    tmin = AddTime(tmax, ImPlotTimeUnit_S, -1);
                axis.SetRange(tmin.ToDouble(),tmax.ToDouble());
            }
            ImGui::Separator();
            if (ShowDatePicker("maxdate",&axis.PickerLevel,&axis.PickerTimeMax,&tmin,&tmax)) {
                tmax = CombineDateTime(axis.PickerTimeMax, tmax);
                if (tmax <= tmin)
                    tmin = AddTime(tmax, ImPlotTimeUnit_S, -1);
                axis.SetRange(tmin.ToDouble(), tmax.ToDouble());
            }
            ImGui::EndMenu();
        }
        EndDisabledControls(axis.IsLockedMax() || always_locked);
    }
    else {
        BeginDisabledControls(always_locked);
        ImGui::CheckboxFlags("##LockMin", (unsigned int*)&axis.Flags, ImPlotAxisFlags_LockMin);
        EndDisabledControls(always_locked);
        ImGui::SameLine();
        BeginDisabledControls(axis.IsLockedMin() || always_locked);
        double temp_min = axis.Range.Min;
        if (DragFloat("Min", &temp_min, (float)drag_speed, -HUGE_VAL, axis.Range.Max - DBL_EPSILON)) {
            axis.SetMin(temp_min,true);
            if (equal_axis != nullptr)
                equal_axis->SetAspect(axis.GetAspect());
        }
        EndDisabledControls(axis.IsLockedMin() || always_locked);

        BeginDisabledControls(always_locked);
        ImGui::CheckboxFlags("##LockMax", (unsigned int*)&axis.Flags, ImPlotAxisFlags_LockMax);
        EndDisabledControls(always_locked);
        ImGui::SameLine();
        BeginDisabledControls(axis.IsLockedMax() || always_locked);
        double temp_max = axis.Range.Max;
        if (DragFloat("Max", &temp_max, (float)drag_speed, axis.Range.Min + DBL_EPSILON, HUGE_VAL)) {
            axis.SetMax(temp_max,true);
            if (equal_axis != nullptr)
                equal_axis->SetAspect(axis.GetAspect());
        }
        EndDisabledControls(axis.IsLockedMax() || always_locked);
    }

    ImGui::Separator();

    ImGui::CheckboxFlags("Auto-Fit",(unsigned int*)&axis.Flags, ImPlotAxisFlags_AutoFit);
    // TODO
    // BeginDisabledControls(axis.IsTime() && time_allowed);
    // ImGui::CheckboxFlags("Log Scale",(unsigned int*)&axis.Flags, ImPlotAxisFlags_LogScale);
    // EndDisabledControls(axis.IsTime() && time_allowed);
    // if (time_allowed) {
    //     BeginDisabledControls(axis.IsLog() || axis.IsSymLog());
    //     ImGui::CheckboxFlags("Time",(unsigned int*)&axis.Flags, ImPlotAxisFlags_Time);
    //     EndDisabledControls(axis.IsLog() || axis.IsSymLog());
    // }
    ImGui::Separator();
    ImGui::CheckboxFlags("Invert",(unsigned int*)&axis.Flags, ImPlotAxisFlags_Invert);
    ImGui::CheckboxFlags("Opposite",(unsigned int*)&axis.Flags, ImPlotAxisFlags_Opposite);
    ImGui::Separator();
    BeginDisabledControls(axis.LabelOffset == -1);
    if (ImGui::Checkbox("Label", &label))
        ImFlipFlag(axis.Flags, ImPlotAxisFlags_NoLabel);
    EndDisabledControls(axis.LabelOffset == -1);
    if (ImGui::Checkbox("Grid Lines", &grid))
        ImFlipFlag(axis.Flags, ImPlotAxisFlags_NoGridLines);
    if (ImGui::Checkbox("Tick Marks", &ticks))
        ImFlipFlag(axis.Flags, ImPlotAxisFlags_NoTickMarks);
    if (ImGui::Checkbox("Tick Labels", &labels))
        ImFlipFlag(axis.Flags, ImPlotAxisFlags_NoTickLabels);

}

bool ShowLegendContextMenu(ImPlotLegend& legend, bool visible) {
    const float s = ImGui::GetFrameHeight();
    bool ret = false;
    if (ImGui::Checkbox("Show",&visible))
        ret = true;
    if (legend.CanGoInside)
        ImGui::CheckboxFlags("Outside",(unsigned int*)&legend.Flags, ImPlotLegendFlags_Outside);
    if (ImGui::RadioButton("H", ImHasFlag(legend.Flags, ImPlotLegendFlags_Horizontal)))
        legend.Flags |= ImPlotLegendFlags_Horizontal;
    ImGui::SameLine();
    if (ImGui::RadioButton("V", !ImHasFlag(legend.Flags, ImPlotLegendFlags_Horizontal)))
        legend.Flags &= ~ImPlotLegendFlags_Horizontal;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2,2));
    if (ImGui::Button("NW",ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_NorthWest; } ImGui::SameLine();
    if (ImGui::Button("N", ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_North;     } ImGui::SameLine();
    if (ImGui::Button("NE",ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_NorthEast; }
    if (ImGui::Button("W", ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_West;      } ImGui::SameLine();
    if (ImGui::InvisibleButton("C", ImVec2(1.5f*s,s))) {     } ImGui::SameLine();
    if (ImGui::Button("E", ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_East;      }
    if (ImGui::Button("SW",ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_SouthWest; } ImGui::SameLine();
    if (ImGui::Button("S", ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_South;     } ImGui::SameLine();
    if (ImGui::Button("SE",ImVec2(1.5f*s,s))) { legend.Location = ImPlotLocation_SouthEast; }
    ImGui::PopStyleVar();
    return ret;
}

void ShowSubplotsContextMenu(ImPlotSubplot& subplot) {
    if ((ImGui::BeginMenu("Linking"))) {
        if (ImGui::MenuItem("Link Rows",nullptr,ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkRows)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_LinkRows);
        if (ImGui::MenuItem("Link Cols",nullptr,ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkCols)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_LinkCols);
        if (ImGui::MenuItem("Link All X",nullptr,ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkAllX)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_LinkAllX);
        if (ImGui::MenuItem("Link All Y",nullptr,ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkAllY)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_LinkAllY);
        ImGui::EndMenu();
    }
    if ((ImGui::BeginMenu("Settings"))) {
        BeginDisabledControls(!subplot.HasTitle);
        if (ImGui::MenuItem("Title",nullptr,subplot.HasTitle && !ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoTitle)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_NoTitle);
        EndDisabledControls(!subplot.HasTitle);
        if (ImGui::MenuItem("Resizable",nullptr,!ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoResize)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_NoResize);
        if (ImGui::MenuItem("Align",nullptr,!ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoAlign)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_NoAlign);
        if (ImGui::MenuItem("Share Items",nullptr,ImHasFlag(subplot.Flags, ImPlotSubplotFlags_ShareItems)))
            ImFlipFlag(subplot.Flags, ImPlotSubplotFlags_ShareItems);
        ImGui::EndMenu();
    }
}

void ShowPlotContextMenu(ImPlotPlot& plot) {
    ImPlotContext& gp = *GImPlot;
    const bool owns_legend = gp.CurrentItems == &plot.Items;
    const bool equal = ImHasFlag(plot.Flags, ImPlotFlags_Equal);

    char buf[16] = {};

    for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
        ImPlotAxis& x_axis = plot.XAxis(i);
        if (!x_axis.Enabled || !x_axis.HasMenus())
            continue;
        ImGui::PushID(i);
        ImFormatString(buf, sizeof(buf) - 1, i == 0 ? "X-Axis" : "X-Axis %d", i + 1);
        if (ImGui::BeginMenu(x_axis.HasLabel() ? plot.GetAxisLabel(x_axis) : buf)) {
            ShowAxisContextMenu(x_axis, equal ? x_axis.OrthoAxis : nullptr, false);
            ImGui::EndMenu();
        }
        ImGui::PopID();
    }

    for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
        ImPlotAxis& y_axis = plot.YAxis(i);
        if (!y_axis.Enabled || !y_axis.HasMenus())
            continue;
        ImGui::PushID(i);
        ImFormatString(buf, sizeof(buf) - 1, i == 0 ? "Y-Axis" : "Y-Axis %d", i + 1);
        if (ImGui::BeginMenu(y_axis.HasLabel() ? plot.GetAxisLabel(y_axis) : buf)) {
            ShowAxisContextMenu(y_axis, equal ? y_axis.OrthoAxis : nullptr, false);
            ImGui::EndMenu();
        }
        ImGui::PopID();
    }

    ImGui::Separator();
    if (!ImHasFlag(gp.CurrentItems->Legend.Flags, ImPlotLegendFlags_NoMenus)) {
        if ((ImGui::BeginMenu("Legend"))) {
            if (owns_legend) {
                if (ShowLegendContextMenu(plot.Items.Legend, !ImHasFlag(plot.Flags, ImPlotFlags_NoLegend)))
                    ImFlipFlag(plot.Flags, ImPlotFlags_NoLegend);
            }
            else if (gp.CurrentSubplot != nullptr) {
                if (ShowLegendContextMenu(gp.CurrentSubplot->Items.Legend, !ImHasFlag(gp.CurrentSubplot->Flags, ImPlotSubplotFlags_NoLegend)))
                    ImFlipFlag(gp.CurrentSubplot->Flags, ImPlotSubplotFlags_NoLegend);
            }
            ImGui::EndMenu();
        }
    }
    if ((ImGui::BeginMenu("Settings"))) {
        if (ImGui::MenuItem("Equal", nullptr, ImHasFlag(plot.Flags, ImPlotFlags_Equal)))
            ImFlipFlag(plot.Flags, ImPlotFlags_Equal);
        if (ImGui::MenuItem("Box Select",nullptr,!ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect)))
            ImFlipFlag(plot.Flags, ImPlotFlags_NoBoxSelect);
        BeginDisabledControls(plot.TitleOffset == -1);
        if (ImGui::MenuItem("Title",nullptr,plot.HasTitle()))
            ImFlipFlag(plot.Flags, ImPlotFlags_NoTitle);
        EndDisabledControls(plot.TitleOffset == -1);
        if (ImGui::MenuItem("Mouse Position",nullptr,!ImHasFlag(plot.Flags, ImPlotFlags_NoMouseText)))
            ImFlipFlag(plot.Flags, ImPlotFlags_NoMouseText);
        if (ImGui::MenuItem("Crosshairs",nullptr,ImHasFlag(plot.Flags, ImPlotFlags_Crosshairs)))
            ImFlipFlag(plot.Flags, ImPlotFlags_Crosshairs);
        ImGui::EndMenu();
    }
    if (gp.CurrentSubplot != nullptr && !ImHasFlag(gp.CurrentSubplot->Flags, ImPlotSubplotFlags_NoMenus)) {
        ImGui::Separator();
        if ((ImGui::BeginMenu("Subplots"))) {
            ShowSubplotsContextMenu(*gp.CurrentSubplot);
            ImGui::EndMenu();
        }
    }
}

//-----------------------------------------------------------------------------
// Axis Utils
//-----------------------------------------------------------------------------

static inline int AxisPrecision(const ImPlotAxis& axis) {
    const double range = axis.Ticker.TickCount() > 1 ? (axis.Ticker.Ticks[1].PlotPos - axis.Ticker.Ticks[0].PlotPos) : axis.Range.Size();
    return Precision(range);
}

static inline double RoundAxisValue(const ImPlotAxis& axis, double value) {
    return RoundTo(value, AxisPrecision(axis));
}

void LabelAxisValue(const ImPlotAxis& axis, double value, char* buff, int size, bool round) {
    ImPlotContext& gp = *GImPlot;
    // TODO: We shouldn't explicitly check that the axis is Time here. Ideally,
    // Formatter_Time would handle the formatting for us, but the code below
    // needs additional arguments which are not currently available in ImPlotFormatter
    if (axis.Locator == Locator_Time) {
        ImPlotTimeUnit unit = axis.Vertical
                            ? GetUnitForRange(axis.Range.Size() / (gp.CurrentPlot->PlotRect.GetHeight() / 100)) // TODO: magic value!
                            : GetUnitForRange(axis.Range.Size() / (gp.CurrentPlot->PlotRect.GetWidth() / 100)); // TODO: magic value!
        FormatDateTime(ImPlotTime::FromDouble(value), buff, size, GetDateTimeFmt(TimeFormatMouseCursor, unit));
    }
    else {
        if (round)
            value = RoundAxisValue(axis, value);
        axis.Formatter(value, buff, size, axis.FormatterData);
    }
}

void UpdateAxisColors(ImPlotAxis& axis) {
    const ImVec4 col_grid = GetStyleColorVec4(ImPlotCol_AxisGrid);
    axis.ColorMaj         = ImGui::GetColorU32(col_grid);
    axis.ColorMin         = ImGui::GetColorU32(col_grid*ImVec4(1,1,1,GImPlot->Style.MinorAlpha));
    axis.ColorTick        = GetStyleColorU32(ImPlotCol_AxisTick);
    axis.ColorTxt         = GetStyleColorU32(ImPlotCol_AxisText);
    axis.ColorBg          = GetStyleColorU32(ImPlotCol_AxisBg);
    axis.ColorHov         = GetStyleColorU32(ImPlotCol_AxisBgHovered);
    axis.ColorAct         = GetStyleColorU32(ImPlotCol_AxisBgActive);
    // axis.ColorHiLi     = IM_COL32_BLACK_TRANS;
}

void PadAndDatumAxesX(ImPlotPlot& plot, float& pad_T, float& pad_B, ImPlotAlignmentData* align) {

    ImPlotContext& gp = *GImPlot;

    const float T = ImGui::GetTextLineHeight();
    const float P = gp.Style.LabelPadding.y;
    const float K = gp.Style.MinorTickLen.x;

    int   count_T = 0;
    int   count_B = 0;
    float last_T  = plot.AxesRect.Min.y;
    float last_B  = plot.AxesRect.Max.y;

    for (int i = IMPLOT_NUM_X_AXES; i-- > 0;) { // FYI: can iterate forward
        ImPlotAxis& axis = plot.XAxis(i);
        if (!axis.Enabled)
            continue;
        const bool label = axis.HasLabel();
        const bool ticks = axis.HasTickLabels();
        const bool opp   = axis.IsOpposite();
        const bool time  = axis.Scale == ImPlotScale_Time;
        if (opp) {
            if (count_T++ > 0)
                pad_T += K + P;
            if (label)
                pad_T += T + P;
            if (ticks)
                pad_T += ImMax(T, axis.Ticker.MaxSize.y) + P + (time ? T + P : 0);
            axis.Datum1 = plot.CanvasRect.Min.y + pad_T;
            axis.Datum2 = last_T;
            last_T = axis.Datum1;
        }
        else {
            if (count_B++ > 0)
                pad_B += K + P;
            if (label)
                pad_B += T + P;
            if (ticks)
                pad_B += ImMax(T, axis.Ticker.MaxSize.y) + P + (time ? T + P : 0);
            axis.Datum1 = plot.CanvasRect.Max.y - pad_B;
            axis.Datum2 = last_B;
            last_B = axis.Datum1;
        }
    }

    if (align) {
        count_T = count_B = 0;
        float delta_T, delta_B;
        align->Update(pad_T,pad_B,delta_T,delta_B);
        for (int i = IMPLOT_NUM_X_AXES; i-- > 0;) {
            ImPlotAxis& axis = plot.XAxis(i);
            if (!axis.Enabled)
                continue;
            if (axis.IsOpposite()) {
                axis.Datum1 += delta_T;
                axis.Datum2 += count_T++ > 1 ? delta_T : 0;
            }
            else {
                axis.Datum1 -= delta_B;
                axis.Datum2 -= count_B++ > 1 ? delta_B : 0;
            }
        }
    }
}

void PadAndDatumAxesY(ImPlotPlot& plot, float& pad_L, float& pad_R, ImPlotAlignmentData* align) {

    //   [   pad_L   ]                 [   pad_R   ]
    //   .................CanvasRect................
    //   :TPWPK.PTPWP _____PlotRect____ PWPTP.KPWPT:
    //   :A # |- A # |-               -| # A -| # A:
    //   :X   |  X   |                 |   X  |   x:
    //   :I # |- I # |-               -| # I -| # I:
    //   :S   |  S   |                 |   S  |   S:
    //   :3 # |- 0 # |-_______________-| # 1 -| # 2:
    //   :.........................................:
    //
    //   T = text height
    //   P = label padding
    //   K = minor tick length
    //   W = label width

    ImPlotContext& gp = *GImPlot;

    const float T = ImGui::GetTextLineHeight();
    const float P = gp.Style.LabelPadding.x;
    const float K = gp.Style.MinorTickLen.y;

    int   count_L = 0;
    int   count_R = 0;
    float last_L  = plot.AxesRect.Min.x;
    float last_R  = plot.AxesRect.Max.x;

    for (int i = IMPLOT_NUM_Y_AXES; i-- > 0;) { // FYI: can iterate forward
        ImPlotAxis& axis = plot.YAxis(i);
        if (!axis.Enabled)
            continue;
        const bool label = axis.HasLabel();
        const bool ticks = axis.HasTickLabels();
        const bool opp   = axis.IsOpposite();
        if (opp) {
            if (count_R++ > 0)
                pad_R += K + P;
            if (label)
                pad_R += T + P;
            if (ticks)
                pad_R += axis.Ticker.MaxSize.x + P;
            axis.Datum1 = plot.CanvasRect.Max.x - pad_R;
            axis.Datum2 = last_R;
            last_R = axis.Datum1;
        }
        else {
            if (count_L++ > 0)
                pad_L += K + P;
            if (label)
                pad_L += T + P;
            if (ticks)
                pad_L += axis.Ticker.MaxSize.x + P;
            axis.Datum1 = plot.CanvasRect.Min.x + pad_L;
            axis.Datum2 = last_L;
            last_L = axis.Datum1;
        }
    }

    plot.PlotRect.Min.x = plot.CanvasRect.Min.x + pad_L;
    plot.PlotRect.Max.x = plot.CanvasRect.Max.x - pad_R;

    if (align) {
        count_L = count_R = 0;
        float delta_L, delta_R;
        align->Update(pad_L,pad_R,delta_L,delta_R);
        for (int i = IMPLOT_NUM_Y_AXES; i-- > 0;) {
            ImPlotAxis& axis = plot.YAxis(i);
            if (!axis.Enabled)
                continue;
            if (axis.IsOpposite()) {
                axis.Datum1 -= delta_R;
                axis.Datum2 -= count_R++ > 1 ? delta_R : 0;
            }
            else {
                axis.Datum1 += delta_L;
                axis.Datum2 += count_L++ > 1 ? delta_L : 0;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// RENDERING
//-----------------------------------------------------------------------------

static inline void RenderGridLinesX(ImDrawList& DrawList, const ImPlotTicker& ticker, const ImRect& rect, ImU32 col_maj, ImU32 col_min, float size_maj, float size_min) {
    const float density   = ticker.TickCount() / rect.GetWidth();
    ImVec4 col_min4  = ImGui::ColorConvertU32ToFloat4(col_min);
    col_min4.w      *= ImClamp(ImRemap(density, 0.1f, 0.2f, 1.0f, 0.0f), 0.0f, 1.0f);
    col_min = ImGui::ColorConvertFloat4ToU32(col_min4);
    for (int t = 0; t < ticker.TickCount(); t++) {
        const ImPlotTick& xt = ticker.Ticks[t];
        if (xt.PixelPos < rect.Min.x || xt.PixelPos > rect.Max.x)
            continue;
        if (xt.Level == 0) {
            if (xt.Major)
                DrawList.AddLine(ImVec2(xt.PixelPos, rect.Min.y), ImVec2(xt.PixelPos, rect.Max.y), col_maj, size_maj);
            else if (density < 0.2f)
                DrawList.AddLine(ImVec2(xt.PixelPos, rect.Min.y), ImVec2(xt.PixelPos, rect.Max.y), col_min, size_min);
        }
    }
}

static inline void RenderGridLinesY(ImDrawList& DrawList, const ImPlotTicker& ticker, const ImRect& rect, ImU32 col_maj, ImU32 col_min, float size_maj, float size_min) {
    const float density   = ticker.TickCount() / rect.GetHeight();
    ImVec4 col_min4  = ImGui::ColorConvertU32ToFloat4(col_min);
    col_min4.w      *= ImClamp(ImRemap(density, 0.1f, 0.2f, 1.0f, 0.0f), 0.0f, 1.0f);
    col_min = ImGui::ColorConvertFloat4ToU32(col_min4);
    for (int t = 0; t < ticker.TickCount(); t++) {
        const ImPlotTick& yt = ticker.Ticks[t];
        if (yt.PixelPos < rect.Min.y || yt.PixelPos > rect.Max.y)
            continue;
        if (yt.Major)
            DrawList.AddLine(ImVec2(rect.Min.x, yt.PixelPos), ImVec2(rect.Max.x, yt.PixelPos), col_maj, size_maj);
        else if (density < 0.2f)
            DrawList.AddLine(ImVec2(rect.Min.x, yt.PixelPos), ImVec2(rect.Max.x, yt.PixelPos), col_min, size_min);
    }
}

static inline void RenderSelectionRect(ImDrawList& DrawList, const ImVec2& p_min, const ImVec2& p_max, const ImVec4& col) {
    const ImU32 col_bg = ImGui::GetColorU32(col * ImVec4(1,1,1,0.25f));
    const ImU32 col_bd = ImGui::GetColorU32(col);
    DrawList.AddRectFilled(p_min, p_max, col_bg);
    DrawList.AddRect(p_min, p_max, col_bd);
}

//-----------------------------------------------------------------------------
// Input Handling
//-----------------------------------------------------------------------------

static const float MOUSE_CURSOR_DRAG_THRESHOLD = 5.0f;
static const float BOX_SELECT_DRAG_THRESHOLD   = 4.0f;

bool UpdateInput(ImPlotPlot& plot) {

    bool changed = false;

    ImPlotContext& gp = *GImPlot;
    ImGuiIO& IO = ImGui::GetIO();

    // BUTTON STATE -----------------------------------------------------------

    const ImGuiButtonFlags plot_button_flags = ImGuiButtonFlags_AllowOverlap
                                             | ImGuiButtonFlags_PressedOnClick
                                             | ImGuiButtonFlags_PressedOnDoubleClick
                                             | ImGuiButtonFlags_MouseButtonLeft
                                             | ImGuiButtonFlags_MouseButtonRight
                                             | ImGuiButtonFlags_MouseButtonMiddle;
    const ImGuiButtonFlags axis_button_flags = ImGuiButtonFlags_FlattenChildren
                                             | plot_button_flags;

    const bool plot_clicked = ImGui::ButtonBehavior(plot.PlotRect,plot.ID,&plot.Hovered,&plot.Held,plot_button_flags);
#if (IMGUI_VERSION_NUM < 18966)
    ImGui::SetItemAllowOverlap(); // Handled by ButtonBehavior()
#endif

    if (plot_clicked) {
        if (!ImHasFlag(plot.Flags, ImPlotFlags_NoBoxSelect) && IO.MouseClicked[gp.InputMap.Select] && ImHasFlag(IO.KeyMods, gp.InputMap.SelectMod)) {
            plot.Selecting   = true;
            plot.SelectStart = IO.MousePos;
            plot.SelectRect  = ImRect(0,0,0,0);
        }
        if (IO.MouseDoubleClicked[gp.InputMap.Fit]) {
            plot.FitThisFrame = true;
            for (int i = 0; i < ImAxis_COUNT; ++i)
                plot.Axes[i].FitThisFrame = true;
        }
    }

    const bool can_pan = IO.MouseDown[gp.InputMap.Pan] && ImHasFlag(IO.KeyMods, gp.InputMap.PanMod);

    plot.Held = plot.Held && can_pan;

    bool x_click[IMPLOT_NUM_X_AXES] = {false};
    bool x_held[IMPLOT_NUM_X_AXES]  = {false};
    bool x_hov[IMPLOT_NUM_X_AXES]   = {false};

    bool y_click[IMPLOT_NUM_Y_AXES] = {false};
    bool y_held[IMPLOT_NUM_Y_AXES]  = {false};
    bool y_hov[IMPLOT_NUM_Y_AXES]   = {false};

    for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
        ImPlotAxis& xax = plot.XAxis(i);
        if (xax.Enabled) {
            ImGui::KeepAliveID(xax.ID);
            x_click[i]  = ImGui::ButtonBehavior(xax.HoverRect,xax.ID,&xax.Hovered,&xax.Held,axis_button_flags);
            if (x_click[i] && IO.MouseDoubleClicked[gp.InputMap.Fit])
                plot.FitThisFrame = xax.FitThisFrame = true;
            xax.Held  = xax.Held && can_pan;
            x_hov[i]  = xax.Hovered || plot.Hovered;
            x_held[i] = xax.Held    || plot.Held;
        }
    }

    for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i) {
        ImPlotAxis& yax = plot.YAxis(i);
        if (yax.Enabled) {
            ImGui::KeepAliveID(yax.ID);
            y_click[i]  = ImGui::ButtonBehavior(yax.HoverRect,yax.ID,&yax.Hovered,&yax.Held,axis_button_flags);
            if (y_click[i] && IO.MouseDoubleClicked[gp.InputMap.Fit])
                plot.FitThisFrame = yax.FitThisFrame = true;
            yax.Held  = yax.Held && can_pan;
            y_hov[i]  = yax.Hovered || plot.Hovered;
            y_held[i] = yax.Held    || plot.Held;
        }
    }

    // cancel due to DND activity
    if (GImGui->DragDropActive || (IO.KeyMods == gp.InputMap.OverrideMod && gp.InputMap.OverrideMod != 0))
        return false;

    // STATE -------------------------------------------------------------------

    const bool axis_equal      = ImHasFlag(plot.Flags, ImPlotFlags_Equal);

    const bool any_x_hov       = plot.Hovered || AnyAxesHovered(&plot.Axes[ImAxis_X1], IMPLOT_NUM_X_AXES);
    const bool any_x_held      = plot.Held    || AnyAxesHeld(&plot.Axes[ImAxis_X1], IMPLOT_NUM_X_AXES);
    const bool any_y_hov       = plot.Hovered || AnyAxesHovered(&plot.Axes[ImAxis_Y1], IMPLOT_NUM_Y_AXES);
    const bool any_y_held      = plot.Held    || AnyAxesHeld(&plot.Axes[ImAxis_Y1], IMPLOT_NUM_Y_AXES);
    const bool any_hov         = any_x_hov    || any_y_hov;
    const bool any_held        = any_x_held   || any_y_held;

    const ImVec2 select_drag   = ImGui::GetMouseDragDelta(gp.InputMap.Select);
    const ImVec2 pan_drag      = ImGui::GetMouseDragDelta(gp.InputMap.Pan);
    const float select_drag_sq = ImLengthSqr(select_drag);
    const float pan_drag_sq    = ImLengthSqr(pan_drag);
    const bool selecting       = plot.Selecting && select_drag_sq > MOUSE_CURSOR_DRAG_THRESHOLD;
    const bool panning         = any_held       && pan_drag_sq    > MOUSE_CURSOR_DRAG_THRESHOLD;

    // CONTEXT MENU -----------------------------------------------------------

    if (IO.MouseReleased[gp.InputMap.Menu] && !plot.ContextLocked)
        gp.OpenContextThisFrame = true;

    if (selecting || panning)
        plot.ContextLocked = true;
    else if (!(IO.MouseDown[gp.InputMap.Menu] || IO.MouseReleased[gp.InputMap.Menu]))
        plot.ContextLocked = false;

    // DRAG INPUT -------------------------------------------------------------

    if (any_held && !plot.Selecting) {
        int drag_direction = 0;
        for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
            ImPlotAxis& x_axis = plot.XAxis(i);
            if (x_held[i] && !x_axis.IsInputLocked()) {
                drag_direction |= (1 << 1);
                bool increasing = x_axis.IsInverted() ? IO.MouseDelta.x > 0 : IO.MouseDelta.x < 0;
                if (IO.MouseDelta.x != 0 && !x_axis.IsPanLocked(increasing)) {
                    const double plot_l = x_axis.PixelsToPlot(plot.PlotRect.Min.x - IO.MouseDelta.x);
                    const double plot_r = x_axis.PixelsToPlot(plot.PlotRect.Max.x - IO.MouseDelta.x);
                    x_axis.SetMin(x_axis.IsInverted() ? plot_r : plot_l);
                    x_axis.SetMax(x_axis.IsInverted() ? plot_l : plot_r);
                    if (axis_equal && x_axis.OrthoAxis != nullptr)
                        x_axis.OrthoAxis->SetAspect(x_axis.GetAspect());
                    changed = true;
                }
            }
        }
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
            ImPlotAxis& y_axis = plot.YAxis(i);
            if (y_held[i] && !y_axis.IsInputLocked()) {
                drag_direction |= (1 << 2);
                bool increasing = y_axis.IsInverted() ? IO.MouseDelta.y < 0 : IO.MouseDelta.y > 0;
                if (IO.MouseDelta.y != 0 && !y_axis.IsPanLocked(increasing)) {
                    const double plot_t = y_axis.PixelsToPlot(plot.PlotRect.Min.y - IO.MouseDelta.y);
                    const double plot_b = y_axis.PixelsToPlot(plot.PlotRect.Max.y - IO.MouseDelta.y);
                    y_axis.SetMin(y_axis.IsInverted() ? plot_t : plot_b);
                    y_axis.SetMax(y_axis.IsInverted() ? plot_b : plot_t);
                    if (axis_equal && y_axis.OrthoAxis != nullptr)
                        y_axis.OrthoAxis->SetAspect(y_axis.GetAspect());
                    changed = true;
                }
            }
        }
        if (IO.MouseDragMaxDistanceSqr[gp.InputMap.Pan] > MOUSE_CURSOR_DRAG_THRESHOLD) {
            switch (drag_direction) {
                case 0        : ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed); break;
                case (1 << 1) : ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);   break;
                case (1 << 2) : ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);   break;
                default       : ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);  break;
            }
        }
    }

    // SCROLL INPUT -----------------------------------------------------------

    if (any_hov && ImHasFlag(IO.KeyMods, gp.InputMap.ZoomMod)) {

        float zoom_rate = gp.InputMap.ZoomRate;
        if (IO.MouseWheel == 0.0f)
            zoom_rate = 0;
        else if (IO.MouseWheel > 0)
            zoom_rate = (-zoom_rate) / (1.0f + (2.0f * zoom_rate));
        ImVec2 rect_size = plot.PlotRect.GetSize();
        float tx = ImRemap(IO.MousePos.x, plot.PlotRect.Min.x, plot.PlotRect.Max.x, 0.0f, 1.0f);
        float ty = ImRemap(IO.MousePos.y, plot.PlotRect.Min.y, plot.PlotRect.Max.y, 0.0f, 1.0f);

        for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
            ImPlotAxis& x_axis = plot.XAxis(i);
            const bool equal_zoom   = axis_equal && x_axis.OrthoAxis != nullptr;
            const bool equal_locked = (equal_zoom != false) && x_axis.OrthoAxis->IsInputLocked();
            if (x_hov[i] && !x_axis.IsInputLocked() && !equal_locked) {
                ImGui::SetKeyOwner(ImGuiKey_MouseWheelY, plot.ID);
                if (zoom_rate != 0.0f) {
                    float correction = (plot.Hovered && equal_zoom) ? 0.5f : 1.0f;
                    const double plot_l = x_axis.PixelsToPlot(plot.PlotRect.Min.x - rect_size.x * tx * zoom_rate * correction);
                    const double plot_r = x_axis.PixelsToPlot(plot.PlotRect.Max.x + rect_size.x * (1 - tx) * zoom_rate * correction);
                    x_axis.SetMin(x_axis.IsInverted() ? plot_r : plot_l);
                    x_axis.SetMax(x_axis.IsInverted() ? plot_l : plot_r);
                    if (axis_equal && x_axis.OrthoAxis != nullptr)
                        x_axis.OrthoAxis->SetAspect(x_axis.GetAspect());
                    changed = true;
                }
            }
        }
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
            ImPlotAxis& y_axis = plot.YAxis(i);
            const bool equal_zoom   = axis_equal && y_axis.OrthoAxis != nullptr;
            const bool equal_locked = equal_zoom && y_axis.OrthoAxis->IsInputLocked();
            if (y_hov[i] && !y_axis.IsInputLocked() && !equal_locked) {
                ImGui::SetKeyOwner(ImGuiKey_MouseWheelY, plot.ID);
                if (zoom_rate != 0.0f) {
                    float correction = (plot.Hovered && equal_zoom) ? 0.5f : 1.0f;
                    const double plot_t = y_axis.PixelsToPlot(plot.PlotRect.Min.y - rect_size.y * ty * zoom_rate * correction);
                    const double plot_b = y_axis.PixelsToPlot(plot.PlotRect.Max.y + rect_size.y * (1 - ty) * zoom_rate * correction);
                    y_axis.SetMin(y_axis.IsInverted() ? plot_t : plot_b);
                    y_axis.SetMax(y_axis.IsInverted() ? plot_b : plot_t);
                    if (axis_equal && y_axis.OrthoAxis != nullptr)
                        y_axis.OrthoAxis->SetAspect(y_axis.GetAspect());
                    changed = true;
                }
            }
        }
    }

    // BOX-SELECTION ----------------------------------------------------------

    if (plot.Selecting) {
        const ImVec2 d = plot.SelectStart - IO.MousePos;
        const bool x_can_change = !ImHasFlag(IO.KeyMods,gp.InputMap.SelectHorzMod) && ImFabs(d.x) > 2;
        const bool y_can_change = !ImHasFlag(IO.KeyMods,gp.InputMap.SelectVertMod) && ImFabs(d.y) > 2;
        // confirm
        if (IO.MouseReleased[gp.InputMap.Select]) {
            for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
                ImPlotAxis& x_axis = plot.XAxis(i);
                if (!x_axis.IsInputLocked() && x_can_change) {
                    const double p1 = x_axis.PixelsToPlot(plot.SelectStart.x);
                    const double p2 = x_axis.PixelsToPlot(IO.MousePos.x);
                    x_axis.SetMin(ImMin(p1, p2));
                    x_axis.SetMax(ImMax(p1, p2));
                    changed = true;
                }
            }
            for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
                ImPlotAxis& y_axis = plot.YAxis(i);
                if (!y_axis.IsInputLocked() && y_can_change) {
                    const double p1 = y_axis.PixelsToPlot(plot.SelectStart.y);
                    const double p2 = y_axis.PixelsToPlot(IO.MousePos.y);
                    y_axis.SetMin(ImMin(p1, p2));
                    y_axis.SetMax(ImMax(p1, p2));
                    changed = true;
                }
            }
            if (x_can_change || y_can_change || (ImHasFlag(IO.KeyMods,gp.InputMap.SelectHorzMod) && ImHasFlag(IO.KeyMods,gp.InputMap.SelectVertMod)))
                gp.OpenContextThisFrame = false;
            plot.Selected = plot.Selecting = false;
        }
        // cancel
        else if (IO.MouseReleased[gp.InputMap.SelectCancel]) {
            plot.Selected = plot.Selecting = false;
            gp.OpenContextThisFrame = false;
        }
        else if (ImLengthSqr(d) > BOX_SELECT_DRAG_THRESHOLD) {
            // bad selection
            if (plot.IsInputLocked()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
                gp.OpenContextThisFrame = false;
                plot.Selected      = false;
            }
            else {
                // TODO: Handle only min or max locked cases
                const bool full_width  = ImHasFlag(IO.KeyMods, gp.InputMap.SelectHorzMod) || AllAxesInputLocked(&plot.Axes[ImAxis_X1], IMPLOT_NUM_X_AXES);
                const bool full_height = ImHasFlag(IO.KeyMods, gp.InputMap.SelectVertMod) || AllAxesInputLocked(&plot.Axes[ImAxis_Y1], IMPLOT_NUM_Y_AXES);
                plot.SelectRect.Min.x = full_width  ? plot.PlotRect.Min.x : ImMin(plot.SelectStart.x, IO.MousePos.x);
                plot.SelectRect.Max.x = full_width  ? plot.PlotRect.Max.x : ImMax(plot.SelectStart.x, IO.MousePos.x);
                plot.SelectRect.Min.y = full_height ? plot.PlotRect.Min.y : ImMin(plot.SelectStart.y, IO.MousePos.y);
                plot.SelectRect.Max.y = full_height ? plot.PlotRect.Max.y : ImMax(plot.SelectStart.y, IO.MousePos.y);
                plot.SelectRect.Min  -= plot.PlotRect.Min;
                plot.SelectRect.Max  -= plot.PlotRect.Min;
                plot.Selected = true;
            }
        }
        else {
            plot.Selected = false;
        }
    }
    return changed;
}

//-----------------------------------------------------------------------------
// Next Plot Data (Legacy)
//-----------------------------------------------------------------------------

void ApplyNextPlotData(ImAxis idx) {
    ImPlotContext& gp = *GImPlot;
    ImPlotPlot& plot  = *gp.CurrentPlot;
    ImPlotAxis& axis  = plot.Axes[idx];
    if (!axis.Enabled)
        return;
    double*     npd_lmin = gp.NextPlotData.LinkedMin[idx];
    double*     npd_lmax = gp.NextPlotData.LinkedMax[idx];
    bool        npd_rngh = gp.NextPlotData.HasRange[idx];
    ImPlotCond  npd_rngc = gp.NextPlotData.RangeCond[idx];
    ImPlotRange     npd_rngv = gp.NextPlotData.Range[idx];
    axis.LinkedMin = npd_lmin;
    axis.LinkedMax = npd_lmax;
    axis.PullLinks();
    if (npd_rngh) {
        if (!plot.Initialized || npd_rngc == ImPlotCond_Always)
            axis.SetRange(npd_rngv);
    }
    axis.HasRange         = npd_rngh;
    axis.RangeCond        = npd_rngc;
}

//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------

void SetupAxis(ImAxis idx, const char* label, ImPlotAxisFlags flags) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    // get plot and axis
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    // set ID
    axis.ID = plot.ID + idx + 1;
    // check and set flags
    if (plot.JustCreated || flags != axis.PreviousFlags)
        axis.Flags = flags;
    axis.PreviousFlags = flags;
    // enable axis
    axis.Enabled = true;
    // set label
    plot.SetAxisLabel(axis,label);
    // cache colors
    UpdateAxisColors(axis);
}

void SetupAxisLimits(ImAxis idx, double min_lim, double max_lim, ImPlotCond cond) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");    // get plot and axis
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    if (!plot.Initialized || cond == ImPlotCond_Always)
        axis.SetRange(min_lim, max_lim);
    axis.HasRange  = true;
    axis.RangeCond = cond;
}

void SetupAxisFormat(ImAxis idx, const char* fmt) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.HasFormatSpec = fmt != nullptr;
    if (fmt != nullptr)
        ImStrncpy(axis.FormatSpec,fmt,sizeof(axis.FormatSpec));
}

void SetupAxisLinks(ImAxis idx, double* min_lnk, double* max_lnk) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.LinkedMin = min_lnk;
    axis.LinkedMax = max_lnk;
    axis.PullLinks();
}

void SetupAxisFormat(ImAxis idx, ImPlotFormatter formatter, void* data) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.Formatter = formatter;
    axis.FormatterData = data;
}

void SetupAxisTicks(ImAxis idx, const double* values, int n_ticks, const char* const labels[], bool show_default) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                        "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.ShowDefaultTicks = show_default;
    AddTicksCustom(values,
                  labels,
                  n_ticks,
                  axis.Ticker,
                  axis.Formatter ? axis.Formatter : Formatter_Default,
                  (axis.Formatter && axis.FormatterData) ? axis.FormatterData : axis.HasFormatSpec ? axis.FormatSpec : (void*)IMPLOT_LABEL_FORMAT);
}

void SetupAxisTicks(ImAxis idx, double v_min, double v_max, int n_ticks, const char* const labels[], bool show_default) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    n_ticks = n_ticks < 2 ? 2 : n_ticks;
    FillRange(gp.TempDouble1, n_ticks, v_min, v_max);
    SetupAxisTicks(idx, gp.TempDouble1.Data, n_ticks, labels, show_default);
}

void SetupAxisScale(ImAxis idx, ImPlotScale scale) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                        "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.Scale = scale;
    switch (scale)
    {
    case ImPlotScale_Time:
        axis.TransformForward = nullptr;
        axis.TransformInverse = nullptr;
        axis.TransformData    = nullptr;
        axis.Locator          = Locator_Time;
        axis.ConstraintRange  = ImPlotRange(IMPLOT_MIN_TIME, IMPLOT_MAX_TIME);
        axis.Ticker.Levels    = 2;
        break;
    case ImPlotScale_Log10:
        axis.TransformForward = TransformForward_Log10;
        axis.TransformInverse = TransformInverse_Log10;
        axis.TransformData    = nullptr;
        axis.Locator          = Locator_Log10;
        axis.ConstraintRange  = ImPlotRange(DBL_MIN, INFINITY);
        break;
    case ImPlotScale_SymLog:
        axis.TransformForward = TransformForward_SymLog;
        axis.TransformInverse = TransformInverse_SymLog;
        axis.TransformData    = nullptr;
        axis.Locator          = Locator_SymLog;
        axis.ConstraintRange  = ImPlotRange(-INFINITY, INFINITY);
        break;
    default:
        axis.TransformForward = nullptr;
        axis.TransformInverse = nullptr;
        axis.TransformData    = nullptr;
        axis.Locator          = nullptr;
        axis.ConstraintRange  = ImPlotRange(-INFINITY, INFINITY);
        break;
    }
}

void SetupAxisScale(ImAxis idx, ImPlotTransform fwd, ImPlotTransform inv, void* data) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                        "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.Scale = IMPLOT_AUTO;
    axis.TransformForward = fwd;
    axis.TransformInverse = inv;
    axis.TransformData = data;
}

void SetupAxisLimitsConstraints(ImAxis idx, double v_min, double v_max) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                        "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.ConstraintRange.Min = v_min;
    axis.ConstraintRange.Max = v_max;
}

void SetupAxisZoomConstraints(ImAxis idx, double z_min, double z_max) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                        "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& axis = plot.Axes[idx];
    IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    axis.ConstraintZoom.Min = z_min;
    axis.ConstraintZoom.Max = z_max;
}

void SetupAxes(const char* x_label, const char* y_label, ImPlotAxisFlags x_flags, ImPlotAxisFlags y_flags) {
    SetupAxis(ImAxis_X1, x_label, x_flags);
    SetupAxis(ImAxis_Y1, y_label, y_flags);
}

void SetupAxesLimits(double x_min, double x_max, double y_min, double y_max, ImPlotCond cond) {
    SetupAxisLimits(ImAxis_X1, x_min, x_max, cond);
    SetupAxisLimits(ImAxis_Y1, y_min, y_max, cond);
}

void SetupLegend(ImPlotLocation location, ImPlotLegendFlags flags) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR((gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked) || (gp.CurrentSubplot != nullptr && gp.CurrentPlot == nullptr),
                         "Setup needs to be called after BeginPlot or BeginSubplots and before any setup locking functions (e.g. PlotX)!");
    if (gp.CurrentItems) {
        ImPlotLegend& legend = gp.CurrentItems->Legend;
        // check and set location
        if (location != legend.PreviousLocation)
            legend.Location = location;
        legend.PreviousLocation = location;
        // check and set flags
        if (flags != legend.PreviousFlags)
            legend.Flags = flags;
        legend.PreviousFlags = flags;
    }
}

void SetupMouseText(ImPlotLocation location, ImPlotMouseTextFlags flags) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr && !gp.CurrentPlot->SetupLocked,
                         "Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
    gp.CurrentPlot->MouseTextLocation = location;
    gp.CurrentPlot->MouseTextFlags = flags;
}

//-----------------------------------------------------------------------------
// SetNext
//-----------------------------------------------------------------------------

void SetNextAxisLimits(ImAxis axis, double v_min, double v_max, ImPlotCond cond) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == nullptr, "SetNextAxisLimits() needs to be called before BeginPlot()!");
    IM_ASSERT(cond == 0 || ImIsPowerOfTwo(cond)); // Make sure the user doesn't attempt to combine multiple condition flags.
    gp.NextPlotData.HasRange[axis]  = true;
    gp.NextPlotData.RangeCond[axis] = cond;
    gp.NextPlotData.Range[axis].Min = v_min;
    gp.NextPlotData.Range[axis].Max = v_max;
}

void SetNextAxisLinks(ImAxis axis, double* link_min, double* link_max) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == nullptr, "SetNextAxisLinks() needs to be called before BeginPlot()!");
    gp.NextPlotData.LinkedMin[axis] = link_min;
    gp.NextPlotData.LinkedMax[axis] = link_max;
}

void SetNextAxisToFit(ImAxis axis) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == nullptr, "SetNextAxisToFit() needs to be called before BeginPlot()!");
    gp.NextPlotData.Fit[axis] = true;
}

void SetNextAxesLimits(double x_min, double x_max, double y_min, double y_max, ImPlotCond cond) {
    SetNextAxisLimits(ImAxis_X1, x_min, x_max, cond);
    SetNextAxisLimits(ImAxis_Y1, y_min, y_max, cond);
}

void SetNextAxesToFit() {
    for (int i = 0; i < ImAxis_COUNT; ++i)
        SetNextAxisToFit(i);
}

//-----------------------------------------------------------------------------
// BeginPlot
//-----------------------------------------------------------------------------

bool BeginPlot(const char* title_id, const ImVec2& size, ImPlotFlags flags) {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot == nullptr, "Mismatched BeginPlot()/EndPlot()!");

    // FRONT MATTER -----------------------------------------------------------

    if (gp.CurrentSubplot != nullptr)
        ImGui::PushID(gp.CurrentSubplot->CurrentIdx);

    // get globals
    ImGuiContext &G          = *GImGui;
    ImGuiWindow* Window      = G.CurrentWindow;

    // skip if needed
    if (Window->SkipItems && !gp.CurrentSubplot) {
        ResetCtxForNextPlot(GImPlot);
        return false;
    }

    // ID and age (TODO: keep track of plot age in frames)
    const ImGuiID ID         = Window->GetID(title_id);
    const bool just_created  = gp.Plots.GetByKey(ID) == nullptr;
    gp.CurrentPlot           = gp.Plots.GetOrAddByKey(ID);

    ImPlotPlot &plot         = *gp.CurrentPlot;
    plot.ID                  = ID;
    plot.Items.ID            = ID - 1;
    plot.JustCreated         = just_created;
    plot.SetupLocked         = false;

    // check flags
    if (plot.JustCreated)
        plot.Flags = flags;
    else if (flags != plot.PreviousFlags)
        plot.Flags = flags;
    plot.PreviousFlags = flags;

    // setup default axes
    if (plot.JustCreated) {
        SetupAxis(ImAxis_X1);
        SetupAxis(ImAxis_Y1);
    }

    // reset axes
    for (int i = 0; i < ImAxis_COUNT; ++i) {
        plot.Axes[i].Reset();
        UpdateAxisColors(plot.Axes[i]);
    }
    // ensure first axes enabled
    plot.Axes[ImAxis_X1].Enabled = true;
    plot.Axes[ImAxis_Y1].Enabled = true;
    // set initial axes
    plot.CurrentX = ImAxis_X1;
    plot.CurrentY = ImAxis_Y1;

    // process next plot data (legacy)
    for (int i = 0; i < ImAxis_COUNT; ++i)
        ApplyNextPlotData(i);

    // clear text buffers
    plot.ClearTextBuffer();
    plot.SetTitle(title_id);

    // set frame size
    ImVec2 frame_size;
    if (gp.CurrentSubplot != nullptr)
        frame_size = gp.CurrentSubplot->CellSize;
    else
        frame_size = ImGui::CalcItemSize(size, gp.Style.PlotDefaultSize.x, gp.Style.PlotDefaultSize.y);

    if (frame_size.x < gp.Style.PlotMinSize.x && (size.x < 0.0f || gp.CurrentSubplot != nullptr))
        frame_size.x = gp.Style.PlotMinSize.x;
    if (frame_size.y < gp.Style.PlotMinSize.y && (size.y < 0.0f || gp.CurrentSubplot != nullptr))
        frame_size.y = gp.Style.PlotMinSize.y;

    plot.FrameRect = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ImGui::ItemSize(plot.FrameRect);
    if (!ImGui::ItemAdd(plot.FrameRect, plot.ID, &plot.FrameRect) && !gp.CurrentSubplot) {
        ResetCtxForNextPlot(GImPlot);
        return false;
    }

    // setup items (or dont)
    if (gp.CurrentItems == nullptr)
        gp.CurrentItems = &plot.Items;

    return true;
}

//-----------------------------------------------------------------------------
// SetupFinish
//-----------------------------------------------------------------------------

void SetupFinish() {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "SetupFinish needs to be called after BeginPlot!");

    ImGuiContext& G         = *GImGui;
    ImDrawList& DrawList    = *G.CurrentWindow->DrawList;
    const ImGuiStyle& Style = G.Style;

    ImPlotPlot &plot  = *gp.CurrentPlot;

    // lock setup
    plot.SetupLocked = true;

    // finalize axes and set default formatter/locator
    for (int i = 0; i < ImAxis_COUNT; ++i) {
        ImPlotAxis& axis = plot.Axes[i];
        if (axis.Enabled) {
            axis.Constrain();
            if (!plot.Initialized && axis.CanInitFit())
                plot.FitThisFrame = axis.FitThisFrame = true;
        }
        if (axis.Formatter == nullptr) {
            axis.Formatter = Formatter_Default;
            if (axis.HasFormatSpec)
                axis.FormatterData = axis.FormatSpec;
            else
                axis.FormatterData = (void*)IMPLOT_LABEL_FORMAT;
        }
        if (axis.Locator == nullptr) {
            axis.Locator = Locator_Default;
        }
    }

    // setup nullptr orthogonal axes
    const bool axis_equal = ImHasFlag(plot.Flags, ImPlotFlags_Equal);
    for (int ix = ImAxis_X1, iy = ImAxis_Y1; ix < ImAxis_Y1 || iy < ImAxis_COUNT; ++ix, ++iy) {
        ImPlotAxis& x_axis = plot.Axes[ix];
        ImPlotAxis& y_axis = plot.Axes[iy];
        if (x_axis.Enabled && y_axis.Enabled) {
            if (x_axis.OrthoAxis == nullptr)
                x_axis.OrthoAxis = &y_axis;
            if (y_axis.OrthoAxis == nullptr)
                y_axis.OrthoAxis = &x_axis;
        }
        else if (x_axis.Enabled)
        {
            if (x_axis.OrthoAxis == nullptr && !axis_equal)
                x_axis.OrthoAxis = &plot.Axes[ImAxis_Y1];
        }
        else if (y_axis.Enabled) {
            if (y_axis.OrthoAxis == nullptr && !axis_equal)
                y_axis.OrthoAxis = &plot.Axes[ImAxis_X1];
        }
    }

    // canvas/axes bb
    plot.CanvasRect = ImRect(plot.FrameRect.Min + gp.Style.PlotPadding, plot.FrameRect.Max - gp.Style.PlotPadding);
    plot.AxesRect   = plot.FrameRect;

    // outside legend adjustments
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoLegend) && plot.Items.GetLegendCount() > 0 && ImHasFlag(plot.Items.Legend.Flags, ImPlotLegendFlags_Outside)) {
        ImPlotLegend& legend = plot.Items.Legend;
        const bool horz = ImHasFlag(legend.Flags, ImPlotLegendFlags_Horizontal);
        const ImVec2 legend_size = CalcLegendSize(plot.Items, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, !horz);
        const bool west = ImHasFlag(legend.Location, ImPlotLocation_West) && !ImHasFlag(legend.Location, ImPlotLocation_East);
        const bool east = ImHasFlag(legend.Location, ImPlotLocation_East) && !ImHasFlag(legend.Location, ImPlotLocation_West);
        const bool north = ImHasFlag(legend.Location, ImPlotLocation_North) && !ImHasFlag(legend.Location, ImPlotLocation_South);
        const bool south = ImHasFlag(legend.Location, ImPlotLocation_South) && !ImHasFlag(legend.Location, ImPlotLocation_North);
        if ((west && !horz) || (west && horz && !north && !south)) {
            plot.CanvasRect.Min.x += (legend_size.x + gp.Style.LegendPadding.x);
            plot.AxesRect.Min.x   += (legend_size.x + gp.Style.PlotPadding.x);
        }
        if ((east && !horz) || (east && horz && !north && !south)) {
            plot.CanvasRect.Max.x -= (legend_size.x + gp.Style.LegendPadding.x);
            plot.AxesRect.Max.x   -= (legend_size.x + gp.Style.PlotPadding.x);
        }
        if ((north && horz) || (north && !horz && !west && !east)) {
            plot.CanvasRect.Min.y += (legend_size.y + gp.Style.LegendPadding.y);
            plot.AxesRect.Min.y   += (legend_size.y + gp.Style.PlotPadding.y);
        }
        if ((south && horz) || (south && !horz && !west && !east)) {
            plot.CanvasRect.Max.y -= (legend_size.y + gp.Style.LegendPadding.y);
            plot.AxesRect.Max.y   -= (legend_size.y + gp.Style.PlotPadding.y);
        }
    }

    // plot bb
    float pad_top = 0, pad_bot = 0, pad_left = 0, pad_right = 0;

    // (0) calc top padding form title
    ImVec2 title_size(0.0f, 0.0f);
    if (plot.HasTitle())
         title_size = ImGui::CalcTextSize(plot.GetTitle(), nullptr, true);
    if (title_size.x > 0) {
        pad_top += title_size.y + gp.Style.LabelPadding.y;
        plot.AxesRect.Min.y += gp.Style.PlotPadding.y + pad_top;
    }

    // (1) calc addition top padding and bot padding
    PadAndDatumAxesX(plot,pad_top,pad_bot,gp.CurrentAlignmentH);

    const float plot_height = plot.CanvasRect.GetHeight() - pad_top - pad_bot;

    // (2) get y tick labels (needed for left/right pad)
    for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
        ImPlotAxis& axis = plot.YAxis(i);
        if (axis.WillRender() && axis.ShowDefaultTicks && plot_height > 0) {
            axis.Locator(axis.Ticker, axis.Range, plot_height, true, axis.Formatter, axis.FormatterData);
        }
    }

    // (3) calc left/right pad
    PadAndDatumAxesY(plot,pad_left,pad_right,gp.CurrentAlignmentV);

    const float plot_width = plot.CanvasRect.GetWidth() - pad_left - pad_right;

    // (4) get x ticks
    for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
        ImPlotAxis& axis = plot.XAxis(i);
        if (axis.WillRender() && axis.ShowDefaultTicks && plot_width > 0) {
            axis.Locator(axis.Ticker, axis.Range, plot_width, false, axis.Formatter, axis.FormatterData);
        }
    }

    // (5) calc plot bb
    plot.PlotRect = ImRect(plot.CanvasRect.Min + ImVec2(pad_left, pad_top), plot.CanvasRect.Max - ImVec2(pad_right, pad_bot));

    // HOVER------------------------------------------------------------

    // axes hover rect, pixel ranges
    for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
        ImPlotAxis& xax = plot.XAxis(i);
        xax.HoverRect   = ImRect(ImVec2(plot.PlotRect.Min.x, ImMin(xax.Datum1,xax.Datum2)),
                                 ImVec2(plot.PlotRect.Max.x, ImMax(xax.Datum1,xax.Datum2)));
        xax.PixelMin    = xax.IsInverted() ? plot.PlotRect.Max.x : plot.PlotRect.Min.x;
        xax.PixelMax    = xax.IsInverted() ? plot.PlotRect.Min.x : plot.PlotRect.Max.x;
        xax.UpdateTransformCache();
    }

    for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i) {
        ImPlotAxis& yax = plot.YAxis(i);
        yax.HoverRect   = ImRect(ImVec2(ImMin(yax.Datum1,yax.Datum2),plot.PlotRect.Min.y),
                                 ImVec2(ImMax(yax.Datum1,yax.Datum2),plot.PlotRect.Max.y));
        yax.PixelMin    = yax.IsInverted() ? plot.PlotRect.Min.y : plot.PlotRect.Max.y;
        yax.PixelMax    = yax.IsInverted() ? plot.PlotRect.Max.y : plot.PlotRect.Min.y;
        yax.UpdateTransformCache();
    }
    // Equal axis constraint. Must happen after we set Pixels
    // constrain equal axes for primary x and y if not approximately equal
    // constrains x to y since x pixel size depends on y labels width, and causes feedback loops in opposite case
    if (axis_equal) {
        for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
            ImPlotAxis& x_axis = plot.XAxis(i);
            if (x_axis.OrthoAxis == nullptr)
                continue;
            double xar = x_axis.GetAspect();
            double yar = x_axis.OrthoAxis->GetAspect();
            // edge case: user has set x range this frame, so fit y to x so that we honor their request for x range
            // NB: because of feedback across several frames, the user's x request may not be perfectly honored
            if (x_axis.HasRange)
                x_axis.OrthoAxis->SetAspect(xar);
            else if (!ImAlmostEqual(xar,yar) && !x_axis.OrthoAxis->IsInputLocked())
                 x_axis.SetAspect(yar);
        }
    }

    // INPUT ------------------------------------------------------------------
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoInputs))
        UpdateInput(plot);

    // fit from FitNextPlotAxes or auto fit
    for (int i = 0; i < ImAxis_COUNT; ++i) {
        if (gp.NextPlotData.Fit[i] || plot.Axes[i].IsAutoFitting()) {
            plot.FitThisFrame = true;
            plot.Axes[i].FitThisFrame = true;
        }
    }

    // RENDER -----------------------------------------------------------------

    const float txt_height = ImGui::GetTextLineHeight();

    // render frame
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoFrame))
        ImGui::RenderFrame(plot.FrameRect.Min, plot.FrameRect.Max, GetStyleColorU32(ImPlotCol_FrameBg), true, Style.FrameRounding);

    // grid bg
    DrawList.AddRectFilled(plot.PlotRect.Min, plot.PlotRect.Max, GetStyleColorU32(ImPlotCol_PlotBg));

    // transform ticks
    for (int i = 0; i < ImAxis_COUNT; i++) {
        ImPlotAxis& axis = plot.Axes[i];
        if (axis.WillRender()) {
            for (int t = 0; t < axis.Ticker.TickCount(); t++) {
                ImPlotTick& tk = axis.Ticker.Ticks[t];
                tk.PixelPos = IM_ROUND(axis.PlotToPixels(tk.PlotPos));
            }
        }
    }

    // render grid (background)
    for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
        ImPlotAxis& x_axis = plot.XAxis(i);
        if (x_axis.Enabled && x_axis.HasGridLines() && !x_axis.IsForeground())
            RenderGridLinesX(DrawList, x_axis.Ticker, plot.PlotRect, x_axis.ColorMaj, x_axis.ColorMin, gp.Style.MajorGridSize.x, gp.Style.MinorGridSize.x);
    }
    for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
        ImPlotAxis& y_axis = plot.YAxis(i);
        if (y_axis.Enabled && y_axis.HasGridLines() && !y_axis.IsForeground())
            RenderGridLinesY(DrawList, y_axis.Ticker, plot.PlotRect,  y_axis.ColorMaj, y_axis.ColorMin, gp.Style.MajorGridSize.y, gp.Style.MinorGridSize.y);
    }

    // render x axis button, label, tick labels
    for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
        ImPlotAxis& ax = plot.XAxis(i);
        if (!ax.Enabled)
            continue;
        if ((ax.Hovered || ax.Held) && !plot.Held && !ImHasFlag(ax.Flags, ImPlotAxisFlags_NoHighlight))
            DrawList.AddRectFilled(ax.HoverRect.Min, ax.HoverRect.Max, ax.Held ? ax.ColorAct : ax.ColorHov);
        else if (ax.ColorHiLi != IM_COL32_BLACK_TRANS) {
            DrawList.AddRectFilled(ax.HoverRect.Min, ax.HoverRect.Max, ax.ColorHiLi);
            ax.ColorHiLi = IM_COL32_BLACK_TRANS;
        }
        else if (ax.ColorBg != IM_COL32_BLACK_TRANS) {
            DrawList.AddRectFilled(ax.HoverRect.Min, ax.HoverRect.Max, ax.ColorBg);
        }
        const ImPlotTicker& tkr = ax.Ticker;
        const bool opp = ax.IsOpposite();
        if (ax.HasLabel()) {
            const char* label        = plot.GetAxisLabel(ax);
            const ImVec2 label_size  = ImGui::CalcTextSize(label);
            const float label_offset = (ax.HasTickLabels() ? tkr.MaxSize.y + gp.Style.LabelPadding.y : 0.0f)
                                     + (tkr.Levels - 1) * (txt_height + gp.Style.LabelPadding.y)
                                     + gp.Style.LabelPadding.y;
            const ImVec2 label_pos(plot.PlotRect.GetCenter().x - label_size.x * 0.5f,
                                   opp ? ax.Datum1 - label_offset - label_size.y : ax.Datum1 + label_offset);
            DrawList.AddText(label_pos, ax.ColorTxt, label);
        }
        if (ax.HasTickLabels()) {
            for (int j = 0; j < tkr.TickCount(); ++j) {
                const ImPlotTick& tk = tkr.Ticks[j];
                const float datum = ax.Datum1 + (opp ? (-gp.Style.LabelPadding.y -txt_height -tk.Level * (txt_height + gp.Style.LabelPadding.y))
                                                     : gp.Style.LabelPadding.y + tk.Level * (txt_height + gp.Style.LabelPadding.y));
                if (tk.ShowLabel && tk.PixelPos >= plot.PlotRect.Min.x - 1 && tk.PixelPos <= plot.PlotRect.Max.x + 1) {
                    ImVec2 start(tk.PixelPos - 0.5f * tk.LabelSize.x, datum);
                    DrawList.AddText(start, ax.ColorTxt, tkr.GetText(j));
                }
            }
        }
    }

    // render y axis button, label, tick labels
    for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
        ImPlotAxis& ax = plot.YAxis(i);
        if (!ax.Enabled)
            continue;
        if ((ax.Hovered || ax.Held) && !plot.Held && !ImHasFlag(ax.Flags, ImPlotAxisFlags_NoHighlight))
            DrawList.AddRectFilled(ax.HoverRect.Min, ax.HoverRect.Max, ax.Held ? ax.ColorAct : ax.ColorHov);
        else if (ax.ColorHiLi != IM_COL32_BLACK_TRANS) {
            DrawList.AddRectFilled(ax.HoverRect.Min, ax.HoverRect.Max, ax.ColorHiLi);
            ax.ColorHiLi = IM_COL32_BLACK_TRANS;
        }
        else if (ax.ColorBg != IM_COL32_BLACK_TRANS) {
            DrawList.AddRectFilled(ax.HoverRect.Min, ax.HoverRect.Max, ax.ColorBg);
        }
        const ImPlotTicker& tkr = ax.Ticker;
        const bool opp = ax.IsOpposite();
        if (ax.HasLabel()) {
            const char* label        = plot.GetAxisLabel(ax);
            const ImVec2 label_size  = CalcTextSizeVertical(label);
            const float label_offset = (ax.HasTickLabels() ? tkr.MaxSize.x + gp.Style.LabelPadding.x : 0.0f)
                                     + gp.Style.LabelPadding.x;
            const ImVec2 label_pos(opp ? ax.Datum1 + label_offset : ax.Datum1 - label_offset - label_size.x,
                                   plot.PlotRect.GetCenter().y + label_size.y * 0.5f);
            AddTextVertical(&DrawList, label_pos, ax.ColorTxt, label);
        }
        if (ax.HasTickLabels()) {
            for (int j = 0; j < tkr.TickCount(); ++j) {
                const ImPlotTick& tk = tkr.Ticks[j];
                const float datum = ax.Datum1 + (opp ? gp.Style.LabelPadding.x : (-gp.Style.LabelPadding.x - tk.LabelSize.x));
                if (tk.ShowLabel && tk.PixelPos >= plot.PlotRect.Min.y - 1 && tk.PixelPos <= plot.PlotRect.Max.y + 1) {
                    ImVec2 start(datum, tk.PixelPos - 0.5f * tk.LabelSize.y);
                    DrawList.AddText(start, ax.ColorTxt, tkr.GetText(j));
                }
            }
        }
    }


    // clear legend (TODO: put elsewhere)
    plot.Items.Legend.Reset();
    // push ID to set item hashes (NB: !!!THIS PROBABLY NEEDS TO BE IN BEGIN PLOT!!!!)
    ImGui::PushOverrideID(gp.CurrentItems->ID);
}

//-----------------------------------------------------------------------------
// EndPlot()
//-----------------------------------------------------------------------------

void EndPlot() {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "Mismatched BeginPlot()/EndPlot()!");

    SetupLock();

    ImGuiContext &G       = *GImGui;
    ImPlotPlot &plot      = *gp.CurrentPlot;
    ImGuiWindow * Window  = G.CurrentWindow;
    ImDrawList & DrawList = *Window->DrawList;
    const ImGuiIO &   IO  = ImGui::GetIO();

    // FINAL RENDER -----------------------------------------------------------

    const bool render_border  = gp.Style.PlotBorderSize > 0 && GetStyleColorVec4(ImPlotCol_PlotBorder).w > 0;
    const bool any_x_held = plot.Held    || AnyAxesHeld(&plot.Axes[ImAxis_X1], IMPLOT_NUM_X_AXES);
    const bool any_y_held = plot.Held    || AnyAxesHeld(&plot.Axes[ImAxis_Y1], IMPLOT_NUM_Y_AXES);

    ImGui::PushClipRect(plot.FrameRect.Min, plot.FrameRect.Max, true);

    // render grid (foreground)
    for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
        ImPlotAxis& x_axis = plot.XAxis(i);
        if (x_axis.Enabled && x_axis.HasGridLines() && x_axis.IsForeground())
            RenderGridLinesX(DrawList, x_axis.Ticker, plot.PlotRect, x_axis.ColorMaj, x_axis.ColorMin, gp.Style.MajorGridSize.x, gp.Style.MinorGridSize.x);
    }
    for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
        ImPlotAxis& y_axis = plot.YAxis(i);
        if (y_axis.Enabled && y_axis.HasGridLines() && y_axis.IsForeground())
            RenderGridLinesY(DrawList, y_axis.Ticker, plot.PlotRect,  y_axis.ColorMaj, y_axis.ColorMin, gp.Style.MajorGridSize.y, gp.Style.MinorGridSize.y);
    }


    // render title
    if (plot.HasTitle()) {
        ImU32 col = GetStyleColorU32(ImPlotCol_TitleText);
        AddTextCentered(&DrawList,ImVec2(plot.PlotRect.GetCenter().x, plot.CanvasRect.Min.y),col,plot.GetTitle());
    }

    // render x ticks
    int count_B = 0, count_T = 0;
    for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
        const ImPlotAxis& ax = plot.XAxis(i);
        if (!ax.Enabled)
            continue;
        const ImPlotTicker& tkr = ax.Ticker;
        const bool opp = ax.IsOpposite();
        const bool aux = ((opp && count_T > 0)||(!opp && count_B > 0));
        if (ax.HasTickMarks()) {
            const float direction = opp ? 1.0f : -1.0f;
            for (int j = 0; j < tkr.TickCount(); ++j) {
                const ImPlotTick& tk = tkr.Ticks[j];
                if (tk.Level != 0 || tk.PixelPos < plot.PlotRect.Min.x || tk.PixelPos > plot.PlotRect.Max.x)
                    continue;
                const ImVec2 start(tk.PixelPos, ax.Datum1);
                const float len = (!aux && tk.Major) ? gp.Style.MajorTickLen.x  : gp.Style.MinorTickLen.x;
                const float thk = (!aux && tk.Major) ? gp.Style.MajorTickSize.x : gp.Style.MinorTickSize.x;
                DrawList.AddLine(start, start + ImVec2(0,direction*len), ax.ColorTick, thk);
            }
            if (aux || !render_border)
                DrawList.AddLine(ImVec2(plot.PlotRect.Min.x,ax.Datum1), ImVec2(plot.PlotRect.Max.x,ax.Datum1), ax.ColorTick, gp.Style.MinorTickSize.x);
        }
        count_B += !opp;
        count_T +=  opp;
    }

    // render y ticks
    int count_L = 0, count_R = 0;
    for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
        const ImPlotAxis& ax = plot.YAxis(i);
        if (!ax.Enabled)
            continue;
        const ImPlotTicker& tkr = ax.Ticker;
        const bool opp = ax.IsOpposite();
        const bool aux = ((opp && count_R > 0)||(!opp && count_L > 0));
        if (ax.HasTickMarks()) {
            const float direction = opp ? -1.0f : 1.0f;
            for (int j = 0; j < tkr.TickCount(); ++j) {
                const ImPlotTick& tk = tkr.Ticks[j];
                if (tk.Level != 0 || tk.PixelPos < plot.PlotRect.Min.y || tk.PixelPos > plot.PlotRect.Max.y)
                    continue;
                const ImVec2 start(ax.Datum1, tk.PixelPos);
                const float len = (!aux && tk.Major) ? gp.Style.MajorTickLen.y  : gp.Style.MinorTickLen.y;
                const float thk = (!aux && tk.Major) ? gp.Style.MajorTickSize.y : gp.Style.MinorTickSize.y;
                DrawList.AddLine(start, start + ImVec2(direction*len,0), ax.ColorTick, thk);
            }
            if (aux || !render_border)
                DrawList.AddLine(ImVec2(ax.Datum1, plot.PlotRect.Min.y), ImVec2(ax.Datum1, plot.PlotRect.Max.y), ax.ColorTick, gp.Style.MinorTickSize.y);
        }
        count_L += !opp;
        count_R +=  opp;
    }
    ImGui::PopClipRect();

    // render annotations
    PushPlotClipRect();
    for (int i = 0; i < gp.Annotations.Size; ++i) {
        const char* txt       = gp.Annotations.GetText(i);
        ImPlotAnnotation& an  = gp.Annotations.Annotations[i];
        const ImVec2 txt_size = ImGui::CalcTextSize(txt);
        const ImVec2 size     = txt_size + gp.Style.AnnotationPadding * 2;
        ImVec2 pos            = an.Pos;
        if (an.Offset.x == 0)
            pos.x -= size.x / 2;
        else if (an.Offset.x > 0)
            pos.x += an.Offset.x;
        else
            pos.x -= size.x - an.Offset.x;
        if (an.Offset.y == 0)
            pos.y -= size.y / 2;
        else if (an.Offset.y > 0)
            pos.y += an.Offset.y;
        else
            pos.y -= size.y - an.Offset.y;
        if (an.Clamp)
            pos = ClampLabelPos(pos, size, plot.PlotRect.Min, plot.PlotRect.Max);
        ImRect rect(pos,pos+size);
        if (an.Offset.x != 0 || an.Offset.y != 0) {
            ImVec2 corners[4] = {rect.GetTL(), rect.GetTR(), rect.GetBR(), rect.GetBL()};
            int min_corner = 0;
            float min_len = FLT_MAX;
            for (int c = 0; c < 4; ++c) {
                float len = ImLengthSqr(an.Pos - corners[c]);
                if (len < min_len) {
                    min_corner = c;
                    min_len = len;
                }
            }
            DrawList.AddLine(an.Pos, corners[min_corner], an.ColorBg);
        }
        DrawList.AddRectFilled(rect.Min, rect.Max, an.ColorBg);
        DrawList.AddText(pos + gp.Style.AnnotationPadding, an.ColorFg, txt);
    }

    // render selection
    if (plot.Selected)
        RenderSelectionRect(DrawList, plot.SelectRect.Min + plot.PlotRect.Min, plot.SelectRect.Max + plot.PlotRect.Min, GetStyleColorVec4(ImPlotCol_Selection));

    // render crosshairs
    if (ImHasFlag(plot.Flags, ImPlotFlags_Crosshairs) && plot.Hovered && !(any_x_held || any_y_held) && !plot.Selecting && !plot.Items.Legend.Hovered) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        ImVec2 xy = IO.MousePos;
        ImVec2 h1(plot.PlotRect.Min.x, xy.y);
        ImVec2 h2(xy.x - 5, xy.y);
        ImVec2 h3(xy.x + 5, xy.y);
        ImVec2 h4(plot.PlotRect.Max.x, xy.y);
        ImVec2 v1(xy.x, plot.PlotRect.Min.y);
        ImVec2 v2(xy.x, xy.y - 5);
        ImVec2 v3(xy.x, xy.y + 5);
        ImVec2 v4(xy.x, plot.PlotRect.Max.y);
        ImU32 col = GetStyleColorU32(ImPlotCol_Crosshairs);
        DrawList.AddLine(h1, h2, col);
        DrawList.AddLine(h3, h4, col);
        DrawList.AddLine(v1, v2, col);
        DrawList.AddLine(v3, v4, col);
    }

    // render mouse pos
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoMouseText) && (plot.Hovered || ImHasFlag(plot.MouseTextFlags, ImPlotMouseTextFlags_ShowAlways))) {

        const bool no_aux = ImHasFlag(plot.MouseTextFlags, ImPlotMouseTextFlags_NoAuxAxes);
        const bool no_fmt = ImHasFlag(plot.MouseTextFlags, ImPlotMouseTextFlags_NoFormat);

        ImGuiTextBuffer& builder = gp.MousePosStringBuilder;
        builder.Buf.shrink(0);
        char buff[IMPLOT_LABEL_MAX_SIZE];

        const int num_x = no_aux ? 1 : IMPLOT_NUM_X_AXES;
        for (int i = 0; i < num_x; ++i) {
            ImPlotAxis& x_axis = plot.XAxis(i);
            if (!x_axis.Enabled)
                continue;
            if (i > 0)
                builder.append(", (");
            double v = x_axis.PixelsToPlot(IO.MousePos.x);
            if (no_fmt)
                Formatter_Default(v,buff,IMPLOT_LABEL_MAX_SIZE,(void*)IMPLOT_LABEL_FORMAT);
            else
                LabelAxisValue(x_axis,v,buff,IMPLOT_LABEL_MAX_SIZE,true);
            builder.append(buff);
            if (i > 0)
                builder.append(")");
        }
        builder.append(", ");
        const int num_y = no_aux ? 1 : IMPLOT_NUM_Y_AXES;
        for (int i = 0; i < num_y; ++i) {
            ImPlotAxis& y_axis = plot.YAxis(i);
            if (!y_axis.Enabled)
                continue;
            if (i > 0)
                builder.append(", (");
            double v = y_axis.PixelsToPlot(IO.MousePos.y);
            if (no_fmt)
                Formatter_Default(v,buff,IMPLOT_LABEL_MAX_SIZE,(void*)IMPLOT_LABEL_FORMAT);
            else
                LabelAxisValue(y_axis,v,buff,IMPLOT_LABEL_MAX_SIZE,true);
            builder.append(buff);
            if (i > 0)
                builder.append(")");
        }

        if (!builder.empty()) {
            const ImVec2 size = ImGui::CalcTextSize(builder.c_str());
            const ImVec2 pos = GetLocationPos(plot.PlotRect, size, plot.MouseTextLocation, gp.Style.MousePosPadding);
            DrawList.AddText(pos, GetStyleColorU32(ImPlotCol_InlayText), builder.c_str());
        }
    }
    PopPlotClipRect();

    // axis side switch
    if (!plot.Held) {
        ImVec2 mouse_pos = ImGui::GetIO().MousePos;
        ImRect trigger_rect = plot.PlotRect;
        trigger_rect.Expand(-10);
        for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
            ImPlotAxis& x_axis = plot.XAxis(i);
            if (ImHasFlag(x_axis.Flags, ImPlotAxisFlags_NoSideSwitch))
                continue;
            if (x_axis.Held && plot.PlotRect.Contains(mouse_pos)) {
                const bool opp = ImHasFlag(x_axis.Flags, ImPlotAxisFlags_Opposite);
                if (!opp) {
                    ImRect rect(plot.PlotRect.Min.x - 5, plot.PlotRect.Min.y - 5,
                                plot.PlotRect.Max.x + 5, plot.PlotRect.Min.y + 5);
                    if (mouse_pos.y < plot.PlotRect.Max.y - 10)
                        DrawList.AddRectFilled(rect.Min, rect.Max, x_axis.ColorHov);
                    if (rect.Contains(mouse_pos))
                        x_axis.Flags |= ImPlotAxisFlags_Opposite;
                }
                else {
                    ImRect rect(plot.PlotRect.Min.x - 5, plot.PlotRect.Max.y - 5,
                                plot.PlotRect.Max.x + 5, plot.PlotRect.Max.y + 5);
                    if (mouse_pos.y > plot.PlotRect.Min.y + 10)
                        DrawList.AddRectFilled(rect.Min, rect.Max, x_axis.ColorHov);
                    if (rect.Contains(mouse_pos))
                        x_axis.Flags &= ~ImPlotAxisFlags_Opposite;
                }
            }
        }
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i) {
            ImPlotAxis& y_axis = plot.YAxis(i);
            if (ImHasFlag(y_axis.Flags, ImPlotAxisFlags_NoSideSwitch))
                continue;
            if (y_axis.Held && plot.PlotRect.Contains(mouse_pos)) {
                const bool opp = ImHasFlag(y_axis.Flags, ImPlotAxisFlags_Opposite);
                if (!opp) {
                    ImRect rect(plot.PlotRect.Max.x - 5, plot.PlotRect.Min.y - 5,
                                plot.PlotRect.Max.x + 5, plot.PlotRect.Max.y + 5);
                    if (mouse_pos.x > plot.PlotRect.Min.x + 10)
                        DrawList.AddRectFilled(rect.Min, rect.Max, y_axis.ColorHov);
                    if (rect.Contains(mouse_pos))
                        y_axis.Flags |= ImPlotAxisFlags_Opposite;
                }
                else {
                    ImRect rect(plot.PlotRect.Min.x - 5, plot.PlotRect.Min.y - 5,
                                plot.PlotRect.Min.x + 5, plot.PlotRect.Max.y + 5);
                    if (mouse_pos.x < plot.PlotRect.Max.x - 10)
                        DrawList.AddRectFilled(rect.Min, rect.Max, y_axis.ColorHov);
                    if (rect.Contains(mouse_pos))
                        y_axis.Flags &= ~ImPlotAxisFlags_Opposite;
                }
            }
        }
    }

    // reset legend hovers
    plot.Items.Legend.Hovered = false;
    for (int i = 0; i < plot.Items.GetItemCount(); ++i)
        plot.Items.GetItemByIndex(i)->LegendHovered = false;
    // render legend
    if (!ImHasFlag(plot.Flags, ImPlotFlags_NoLegend) && plot.Items.GetLegendCount() > 0) {
        ImPlotLegend& legend = plot.Items.Legend;
        const bool   legend_out  = ImHasFlag(legend.Flags, ImPlotLegendFlags_Outside);
        const bool   legend_horz = ImHasFlag(legend.Flags, ImPlotLegendFlags_Horizontal);
        const ImVec2 legend_size = CalcLegendSize(plot.Items, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, !legend_horz);
        const ImVec2 legend_pos  = GetLocationPos(legend_out ? plot.FrameRect : plot.PlotRect,
                                                  legend_size,
                                                  legend.Location,
                                                  legend_out ? gp.Style.PlotPadding : gp.Style.LegendPadding);
        legend.Rect = ImRect(legend_pos, legend_pos + legend_size);
        legend.RectClamped = legend.Rect;
        const bool legend_scrollable = ClampLegendRect(legend.RectClamped,
                                                        legend_out ? plot.FrameRect : plot.PlotRect,
                                                        legend_out ? gp.Style.PlotPadding : gp.Style.LegendPadding
                                                        );
        const ImGuiButtonFlags legend_button_flags = ImGuiButtonFlags_AllowOverlap
                                                    | ImGuiButtonFlags_PressedOnClick
                                                    | ImGuiButtonFlags_PressedOnDoubleClick
                                                    | ImGuiButtonFlags_MouseButtonLeft
                                                    | ImGuiButtonFlags_MouseButtonRight
                                                    | ImGuiButtonFlags_MouseButtonMiddle
                                                    | ImGuiButtonFlags_FlattenChildren;
        ImGui::KeepAliveID(plot.Items.ID);
        ImGui::ButtonBehavior(legend.RectClamped, plot.Items.ID, &legend.Hovered, &legend.Held, legend_button_flags);
        legend.Hovered = legend.Hovered || (ImGui::IsWindowHovered() && legend.RectClamped.Contains(IO.MousePos));

        if (legend_scrollable) {
            if (legend.Hovered) {
                ImGui::SetKeyOwner(ImGuiKey_MouseWheelY, plot.Items.ID);
                if (IO.MouseWheel != 0.0f) {
                    ImVec2 max_step = legend.Rect.GetSize() * 0.67f;
                    float font_size = ImGui::GetCurrentWindow()->CalcFontSize();
                    float scroll_step = ImFloor(ImMin(2 * font_size, max_step.x));
                    legend.Scroll.x += scroll_step * IO.MouseWheel;
                    legend.Scroll.y += scroll_step * IO.MouseWheel;
                }
            }
            const ImVec2 min_scroll_offset = legend.RectClamped.GetSize() - legend.Rect.GetSize();
            legend.Scroll.x = ImClamp(legend.Scroll.x, min_scroll_offset.x, 0.0f);
            legend.Scroll.y = ImClamp(legend.Scroll.y, min_scroll_offset.y, 0.0f);
            const ImVec2 scroll_offset = legend_horz ? ImVec2(legend.Scroll.x, 0) : ImVec2(0, legend.Scroll.y);
            ImVec2 legend_offset = legend.RectClamped.Min - legend.Rect.Min + scroll_offset;
            legend.Rect.Min += legend_offset;
            legend.Rect.Max += legend_offset;
        } else {
            legend.Scroll = ImVec2(0,0);
        }

        const ImU32 col_bg  = GetStyleColorU32(ImPlotCol_LegendBg);
        const ImU32 col_bd  = GetStyleColorU32(ImPlotCol_LegendBorder);
        ImGui::PushClipRect(legend.RectClamped.Min, legend.RectClamped.Max, true);
        DrawList.AddRectFilled(legend.RectClamped.Min, legend.RectClamped.Max, col_bg);
        bool legend_contextable = ShowLegendEntries(plot.Items, legend.Rect, legend.Hovered, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, !legend_horz, DrawList)
                                && !ImHasFlag(legend.Flags, ImPlotLegendFlags_NoMenus);
        DrawList.AddRect(legend.RectClamped.Min, legend.RectClamped.Max, col_bd);
        ImGui::PopClipRect();

        // main ctx menu
        if (gp.OpenContextThisFrame && legend_contextable && !ImHasFlag(plot.Flags, ImPlotFlags_NoMenus))
            ImGui::OpenPopup("##LegendContext");

        if (ImGui::BeginPopup("##LegendContext")) {
            ImGui::Text("Legend"); ImGui::Separator();
            if (ShowLegendContextMenu(legend, !ImHasFlag(plot.Flags, ImPlotFlags_NoLegend)))
                ImFlipFlag(plot.Flags, ImPlotFlags_NoLegend);
            ImGui::EndPopup();
        }
    }
    else {
        plot.Items.Legend.Rect = ImRect();
    }

    // render border
    if (render_border)
        DrawList.AddRect(plot.PlotRect.Min, plot.PlotRect.Max, GetStyleColorU32(ImPlotCol_PlotBorder), 0, ImDrawFlags_RoundCornersAll, gp.Style.PlotBorderSize);

    // render tags
    for (int i = 0; i < gp.Tags.Size; ++i) {
        ImPlotTag& tag  = gp.Tags.Tags[i];
        ImPlotAxis& axis = plot.Axes[tag.Axis];
        if (!axis.Enabled || !axis.Range.Contains(tag.Value))
            continue;
        const char* txt = gp.Tags.GetText(i);
        ImVec2 text_size = ImGui::CalcTextSize(txt);
        ImVec2 size = text_size + gp.Style.AnnotationPadding * 2;
        ImVec2 pos;
        axis.Ticker.OverrideSizeLate(size);
        float pix = IM_ROUND(axis.PlotToPixels(tag.Value));
        if (axis.Vertical) {
            if (axis.IsOpposite()) {
                pos = ImVec2(axis.Datum1 + gp.Style.LabelPadding.x, pix - size.y * 0.5f);
                DrawList.AddTriangleFilled(ImVec2(axis.Datum1,pix), pos, pos + ImVec2(0,size.y), tag.ColorBg);
            }
            else {
                pos = ImVec2(axis.Datum1 - size.x - gp.Style.LabelPadding.x, pix - size.y * 0.5f);
                DrawList.AddTriangleFilled(pos + ImVec2(size.x,0), ImVec2(axis.Datum1,pix), pos+size, tag.ColorBg);
            }
        }
        else {
            if (axis.IsOpposite()) {
                pos = ImVec2(pix - size.x * 0.5f, axis.Datum1 - size.y - gp.Style.LabelPadding.y );
                DrawList.AddTriangleFilled(pos + ImVec2(0,size.y), pos + size, ImVec2(pix,axis.Datum1), tag.ColorBg);
            }
            else {
                pos = ImVec2(pix - size.x * 0.5f, axis.Datum1 + gp.Style.LabelPadding.y);
                DrawList.AddTriangleFilled(pos, ImVec2(pix,axis.Datum1), pos + ImVec2(size.x, 0), tag.ColorBg);
            }
        }
        DrawList.AddRectFilled(pos,pos+size,tag.ColorBg);
        DrawList.AddText(pos+gp.Style.AnnotationPadding,tag.ColorFg,txt);
    }

    // FIT DATA --------------------------------------------------------------
    const bool axis_equal = ImHasFlag(plot.Flags, ImPlotFlags_Equal);
    if (plot.FitThisFrame) {
        for (int i = 0; i < IMPLOT_NUM_X_AXES; i++) {
            ImPlotAxis& x_axis = plot.XAxis(i);
            if (x_axis.FitThisFrame) {
                x_axis.ApplyFit(gp.Style.FitPadding.x);
                if (axis_equal && x_axis.OrthoAxis != nullptr) {
                    double aspect = x_axis.GetAspect();
                    ImPlotAxis& y_axis = *x_axis.OrthoAxis;
                    if (y_axis.FitThisFrame) {
                        y_axis.ApplyFit(gp.Style.FitPadding.y);
                        y_axis.FitThisFrame = false;
                        aspect = ImMax(aspect, y_axis.GetAspect());
                    }
                    x_axis.SetAspect(aspect);
                    y_axis.SetAspect(aspect);
                }
            }
        }
        for (int i = 0; i < IMPLOT_NUM_Y_AXES; i++) {
            ImPlotAxis& y_axis = plot.YAxis(i);
            if (y_axis.FitThisFrame) {
                y_axis.ApplyFit(gp.Style.FitPadding.y);
                if (axis_equal && y_axis.OrthoAxis != nullptr) {
                    double aspect = y_axis.GetAspect();
                    ImPlotAxis& x_axis = *y_axis.OrthoAxis;
                    if (x_axis.FitThisFrame) {
                        x_axis.ApplyFit(gp.Style.FitPadding.x);
                        x_axis.FitThisFrame = false;
                        aspect = ImMax(x_axis.GetAspect(), aspect);
                    }
                    x_axis.SetAspect(aspect);
                    y_axis.SetAspect(aspect);
                }
            }
        }
        plot.FitThisFrame = false;
    }

    // CONTEXT MENUS -----------------------------------------------------------

    ImGui::PushOverrideID(plot.ID);

    const bool can_ctx = gp.OpenContextThisFrame &&
                         !ImHasFlag(plot.Flags, ImPlotFlags_NoMenus) &&
                         !plot.Items.Legend.Hovered;



    // main ctx menu
    if (can_ctx && plot.Hovered)
        ImGui::OpenPopup("##PlotContext");
    if (ImGui::BeginPopup("##PlotContext")) {
        ShowPlotContextMenu(plot);
        ImGui::EndPopup();
    }

    // axes ctx menus
    for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
        ImGui::PushID(i);
        ImPlotAxis& x_axis = plot.XAxis(i);
        if (can_ctx && x_axis.Hovered && x_axis.HasMenus())
            ImGui::OpenPopup("##XContext");
        if (ImGui::BeginPopup("##XContext")) {
            ImGui::Text(x_axis.HasLabel() ? plot.GetAxisLabel(x_axis) :  i == 0 ? "X-Axis" : "X-Axis %d", i + 1);
            ImGui::Separator();
            ShowAxisContextMenu(x_axis, axis_equal ? x_axis.OrthoAxis : nullptr, true);
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }
    for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i) {
        ImGui::PushID(i);
        ImPlotAxis& y_axis = plot.YAxis(i);
        if (can_ctx && y_axis.Hovered && y_axis.HasMenus())
            ImGui::OpenPopup("##YContext");
        if (ImGui::BeginPopup("##YContext")) {
            ImGui::Text(y_axis.HasLabel() ? plot.GetAxisLabel(y_axis) : i == 0 ? "Y-Axis" : "Y-Axis %d", i + 1);
            ImGui::Separator();
            ShowAxisContextMenu(y_axis, axis_equal ? y_axis.OrthoAxis : nullptr, false);
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }
    ImGui::PopID();

    // LINKED AXES ------------------------------------------------------------

    for (int i = 0; i < ImAxis_COUNT; ++i)
        plot.Axes[i].PushLinks();


    // CLEANUP ----------------------------------------------------------------

    // remove items
    if (gp.CurrentItems == &plot.Items)
        gp.CurrentItems = nullptr;
    // reset the plot items for the next frame
    for (int i = 0; i < plot.Items.GetItemCount(); ++i) {
        plot.Items.GetItemByIndex(i)->SeenThisFrame = false;
    }

    // mark the plot as initialized, i.e. having made it through one frame completely
    plot.Initialized = true;
    // Pop ImGui::PushID at the end of BeginPlot
    ImGui::PopID();
    // Reset context for next plot
    ResetCtxForNextPlot(GImPlot);

    // setup next subplot
    if (gp.CurrentSubplot != nullptr) {
        ImGui::PopID();
        SubplotNextCell();
    }
}

//-----------------------------------------------------------------------------
// BEGIN/END SUBPLOT
//-----------------------------------------------------------------------------

static const float SUBPLOT_BORDER_SIZE             = 1.0f;
static const float SUBPLOT_SPLITTER_HALF_THICKNESS = 4.0f;
static const float SUBPLOT_SPLITTER_FEEDBACK_TIMER = 0.06f;

void SubplotSetCell(int row, int col) {
    ImPlotContext& gp      = *GImPlot;
    ImPlotSubplot& subplot = *gp.CurrentSubplot;
    if (row >= subplot.Rows || col >= subplot.Cols)
        return;
    float xoff = 0;
    float yoff = 0;
    for (int c = 0; c < col; ++c)
        xoff += subplot.ColRatios[c];
    for (int r = 0; r < row; ++r)
        yoff += subplot.RowRatios[r];
    const ImVec2 grid_size = subplot.GridRect.GetSize();
    ImVec2 cpos            = subplot.GridRect.Min + ImVec2(xoff*grid_size.x,yoff*grid_size.y);
    cpos.x = IM_ROUND(cpos.x);
    cpos.y = IM_ROUND(cpos.y);
    ImGui::GetCurrentWindow()->DC.CursorPos =  cpos;
    // set cell size
    subplot.CellSize.x = IM_ROUND(subplot.GridRect.GetWidth()  * subplot.ColRatios[col]);
    subplot.CellSize.y = IM_ROUND(subplot.GridRect.GetHeight() * subplot.RowRatios[row]);
    // setup links
    const bool lx = ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkAllX);
    const bool ly = ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkAllY);
    const bool lr = ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkRows);
    const bool lc = ImHasFlag(subplot.Flags, ImPlotSubplotFlags_LinkCols);

    SetNextAxisLinks(ImAxis_X1, lx ? &subplot.ColLinkData[0].Min : lc ? &subplot.ColLinkData[col].Min : nullptr,
                                lx ? &subplot.ColLinkData[0].Max : lc ? &subplot.ColLinkData[col].Max : nullptr);
    SetNextAxisLinks(ImAxis_Y1, ly ? &subplot.RowLinkData[0].Min : lr ? &subplot.RowLinkData[row].Min : nullptr,
                                ly ? &subplot.RowLinkData[0].Max : lr ? &subplot.RowLinkData[row].Max : nullptr);
    // setup alignment
    if (!ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoAlign)) {
        gp.CurrentAlignmentH = &subplot.RowAlignmentData[row];
        gp.CurrentAlignmentV = &subplot.ColAlignmentData[col];
    }
    // set idx
    if (ImHasFlag(subplot.Flags, ImPlotSubplotFlags_ColMajor))
        subplot.CurrentIdx = col * subplot.Rows + row;
    else
        subplot.CurrentIdx = row * subplot.Cols + col;
}

void SubplotSetCell(int idx) {
    ImPlotContext& gp      = *GImPlot;
    ImPlotSubplot& subplot = *gp.CurrentSubplot;
    if (idx >= subplot.Rows * subplot.Cols)
        return;
    int row = 0, col = 0;
    if (ImHasFlag(subplot.Flags, ImPlotSubplotFlags_ColMajor)) {
        row = idx % subplot.Rows;
        col = idx / subplot.Rows;
    }
    else {
        row = idx / subplot.Cols;
        col = idx % subplot.Cols;
    }
    return SubplotSetCell(row, col);
}

void SubplotNextCell() {
    ImPlotContext& gp      = *GImPlot;
    ImPlotSubplot& subplot = *gp.CurrentSubplot;
    SubplotSetCell(++subplot.CurrentIdx);
}

bool BeginSubplots(const char* title, int rows, int cols, const ImVec2& size, ImPlotSubplotFlags flags, float* row_sizes, float* col_sizes) {
    IM_ASSERT_USER_ERROR(rows > 0 && cols > 0, "Invalid sizing arguments!");
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentSubplot == nullptr, "Mismatched BeginSubplots()/EndSubplots()!");
    ImGuiContext &G = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    const ImGuiID ID = Window->GetID(title);
    bool just_created = gp.Subplots.GetByKey(ID) == nullptr;
    gp.CurrentSubplot = gp.Subplots.GetOrAddByKey(ID);
    ImPlotSubplot& subplot = *gp.CurrentSubplot;
    subplot.ID       = ID;
    subplot.Items.ID = ID - 1;
    subplot.HasTitle = ImGui::FindRenderedTextEnd(title, nullptr) != title;
    // push ID
    ImGui::PushID(ID);

    if (just_created)
        subplot.Flags = flags;
    else if (flags != subplot.PreviousFlags)
        subplot.Flags = flags;
    subplot.PreviousFlags = flags;

    // check for change in rows and cols
    if (subplot.Rows != rows || subplot.Cols != cols) {
        subplot.RowAlignmentData.resize(rows);
        subplot.RowLinkData.resize(rows);
        subplot.RowRatios.resize(rows);
        for (int r = 0; r < rows; ++r) {
            subplot.RowAlignmentData[r].Reset();
            subplot.RowLinkData[r] = ImPlotRange(0,1);
            subplot.RowRatios[r] = 1.0f / rows;
        }
        subplot.ColAlignmentData.resize(cols);
        subplot.ColLinkData.resize(cols);
        subplot.ColRatios.resize(cols);
        for (int c = 0; c < cols; ++c) {
            subplot.ColAlignmentData[c].Reset();
            subplot.ColLinkData[c] = ImPlotRange(0,1);
            subplot.ColRatios[c] = 1.0f / cols;
        }
    }
    // check incoming size requests
    float row_sum = 0, col_sum = 0;
    if (row_sizes != nullptr) {
        row_sum = ImSum(row_sizes, rows);
        for (int r = 0; r < rows; ++r)
            subplot.RowRatios[r] = row_sizes[r] / row_sum;
    }
    if (col_sizes != nullptr) {
        col_sum = ImSum(col_sizes, cols);
        for (int c = 0; c < cols; ++c)
            subplot.ColRatios[c] = col_sizes[c] / col_sum;
    }
    subplot.Rows = rows;
    subplot.Cols = cols;

    // calc plot frame sizes
    ImVec2 title_size(0.0f, 0.0f);
    if (!ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoTitle))
         title_size = ImGui::CalcTextSize(title, nullptr, true);
    const float pad_top = title_size.x > 0.0f ? title_size.y + gp.Style.LabelPadding.y : 0;
    const ImVec2 half_pad = gp.Style.PlotPadding/2;
    const ImVec2 frame_size = ImGui::CalcItemSize(size, gp.Style.PlotDefaultSize.x, gp.Style.PlotDefaultSize.y);
    subplot.FrameRect = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    subplot.GridRect.Min = subplot.FrameRect.Min + half_pad + ImVec2(0,pad_top);
    subplot.GridRect.Max = subplot.FrameRect.Max - half_pad;
    subplot.FrameHovered = subplot.FrameRect.Contains(ImGui::GetMousePos()) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows|ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    // outside legend adjustments (TODO: make function)
    const bool share_items = ImHasFlag(subplot.Flags, ImPlotSubplotFlags_ShareItems);
    if (share_items)
        gp.CurrentItems = &subplot.Items;
    if (share_items && !ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoLegend) && subplot.Items.GetLegendCount() > 0) {
        ImPlotLegend& legend = subplot.Items.Legend;
        const bool horz = ImHasFlag(legend.Flags, ImPlotLegendFlags_Horizontal);
        const ImVec2 legend_size = CalcLegendSize(subplot.Items, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, !horz);
        const bool west = ImHasFlag(legend.Location, ImPlotLocation_West) && !ImHasFlag(legend.Location, ImPlotLocation_East);
        const bool east = ImHasFlag(legend.Location, ImPlotLocation_East) && !ImHasFlag(legend.Location, ImPlotLocation_West);
        const bool north = ImHasFlag(legend.Location, ImPlotLocation_North) && !ImHasFlag(legend.Location, ImPlotLocation_South);
        const bool south = ImHasFlag(legend.Location, ImPlotLocation_South) && !ImHasFlag(legend.Location, ImPlotLocation_North);
        if ((west && !horz) || (west && horz && !north && !south))
            subplot.GridRect.Min.x += (legend_size.x + gp.Style.LegendPadding.x);
        if ((east && !horz) || (east && horz && !north && !south))
            subplot.GridRect.Max.x -= (legend_size.x + gp.Style.LegendPadding.x);
        if ((north && horz) || (north && !horz && !west && !east))
            subplot.GridRect.Min.y += (legend_size.y + gp.Style.LegendPadding.y);
        if ((south && horz) || (south && !horz && !west && !east))
            subplot.GridRect.Max.y -= (legend_size.y + gp.Style.LegendPadding.y);
    }

    // render single background frame
    ImGui::RenderFrame(subplot.FrameRect.Min, subplot.FrameRect.Max, GetStyleColorU32(ImPlotCol_FrameBg), true, ImGui::GetStyle().FrameRounding);
    // render title
    if (title_size.x > 0.0f && !ImHasFlag(subplot.Flags, ImPlotFlags_NoTitle)) {
        const ImU32 col = GetStyleColorU32(ImPlotCol_TitleText);
        AddTextCentered(ImGui::GetWindowDrawList(),ImVec2(subplot.GridRect.GetCenter().x, subplot.GridRect.Min.y - pad_top + half_pad.y),col,title);
    }

    // render splitters
    if (!ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoResize)) {
        ImDrawList& DrawList = *ImGui::GetWindowDrawList();
        const ImU32 hov_col = ImGui::ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_SeparatorHovered]);
        const ImU32 act_col = ImGui::ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_SeparatorActive]);
        float xpos = subplot.GridRect.Min.x;
        float ypos = subplot.GridRect.Min.y;
        int separator = 1;
        // bool pass = false;
        for (int r = 0; r < subplot.Rows-1; ++r) {
            ypos += subplot.RowRatios[r] * subplot.GridRect.GetHeight();
            const ImGuiID sep_id = subplot.ID + separator;
            ImGui::KeepAliveID(sep_id);
            const ImRect sep_bb = ImRect(subplot.GridRect.Min.x, ypos-SUBPLOT_SPLITTER_HALF_THICKNESS, subplot.GridRect.Max.x, ypos+SUBPLOT_SPLITTER_HALF_THICKNESS);
            bool sep_hov = false, sep_hld = false;
            const bool sep_clk = ImGui::ButtonBehavior(sep_bb, sep_id, &sep_hov, &sep_hld, ImGuiButtonFlags_FlattenChildren | ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_PressedOnDoubleClick);
            if ((sep_hov && G.HoveredIdTimer > SUBPLOT_SPLITTER_FEEDBACK_TIMER) || sep_hld) {
                if (sep_clk && ImGui::IsMouseDoubleClicked(0)) {
                    float p = (subplot.RowRatios[r] + subplot.RowRatios[r+1])/2;
                    subplot.RowRatios[r] = subplot.RowRatios[r+1] = p;
                }
                if (sep_clk) {
                    subplot.TempSizes[0] = subplot.RowRatios[r];
                    subplot.TempSizes[1] = subplot.RowRatios[r+1];
                }
                if (sep_hld) {
                    float dp = ImGui::GetMouseDragDelta(0).y  / subplot.GridRect.GetHeight();
                    if (subplot.TempSizes[0] + dp > 0.1f && subplot.TempSizes[1] - dp > 0.1f) {
                        subplot.RowRatios[r]   = subplot.TempSizes[0] + dp;
                        subplot.RowRatios[r+1] = subplot.TempSizes[1] - dp;
                    }
                }
                DrawList.AddLine(ImVec2(IM_ROUND(subplot.GridRect.Min.x),IM_ROUND(ypos)),
                                 ImVec2(IM_ROUND(subplot.GridRect.Max.x),IM_ROUND(ypos)),
                                 sep_hld ? act_col : hov_col, SUBPLOT_BORDER_SIZE);
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            }
            separator++;
        }
        for (int c = 0; c < subplot.Cols-1; ++c) {
            xpos += subplot.ColRatios[c] * subplot.GridRect.GetWidth();
            const ImGuiID sep_id = subplot.ID + separator;
            ImGui::KeepAliveID(sep_id);
            const ImRect sep_bb = ImRect(xpos-SUBPLOT_SPLITTER_HALF_THICKNESS, subplot.GridRect.Min.y, xpos+SUBPLOT_SPLITTER_HALF_THICKNESS, subplot.GridRect.Max.y);
            bool sep_hov = false, sep_hld = false;
            const bool sep_clk = ImGui::ButtonBehavior(sep_bb, sep_id, &sep_hov, &sep_hld, ImGuiButtonFlags_FlattenChildren | ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_PressedOnDoubleClick);
            if ((sep_hov && G.HoveredIdTimer > SUBPLOT_SPLITTER_FEEDBACK_TIMER) || sep_hld) {
                if (sep_clk && ImGui::IsMouseDoubleClicked(0)) {
                    float p = (subplot.ColRatios[c] + subplot.ColRatios[c+1])/2;
                    subplot.ColRatios[c] = subplot.ColRatios[c+1] = p;
                }
                if (sep_clk) {
                    subplot.TempSizes[0] = subplot.ColRatios[c];
                    subplot.TempSizes[1] = subplot.ColRatios[c+1];
                }
                if (sep_hld) {
                    float dp = ImGui::GetMouseDragDelta(0).x / subplot.GridRect.GetWidth();
                    if (subplot.TempSizes[0] + dp > 0.1f && subplot.TempSizes[1] - dp > 0.1f) {
                        subplot.ColRatios[c]   = subplot.TempSizes[0] + dp;
                        subplot.ColRatios[c+1] = subplot.TempSizes[1] - dp;
                    }
                }
                DrawList.AddLine(ImVec2(IM_ROUND(xpos),IM_ROUND(subplot.GridRect.Min.y)),
                                 ImVec2(IM_ROUND(xpos),IM_ROUND(subplot.GridRect.Max.y)),
                                 sep_hld ? act_col : hov_col, SUBPLOT_BORDER_SIZE);
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            }
            separator++;
        }
    }

    // set outgoing sizes
    if (row_sizes != nullptr) {
        for (int r = 0; r < rows; ++r)
            row_sizes[r] = subplot.RowRatios[r] * row_sum;
    }
    if (col_sizes != nullptr) {
        for (int c = 0; c < cols; ++c)
            col_sizes[c] = subplot.ColRatios[c] * col_sum;
    }

    // push styling
    PushStyleColor(ImPlotCol_FrameBg, IM_COL32_BLACK_TRANS);
    PushStyleVar(ImPlotStyleVar_PlotPadding, half_pad);
    PushStyleVar(ImPlotStyleVar_PlotMinSize, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,0);

    // set initial cursor pos
    Window->DC.CursorPos = subplot.GridRect.Min;
    // begin alignments
    for (int r = 0; r < subplot.Rows; ++r)
        subplot.RowAlignmentData[r].Begin();
    for (int c = 0; c < subplot.Cols; ++c)
        subplot.ColAlignmentData[c].Begin();
    // clear legend data
    subplot.Items.Legend.Reset();
    // Setup first subplot
    SubplotSetCell(0,0);
    return true;
}

void EndSubplots() {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentSubplot != nullptr, "Mismatched BeginSubplots()/EndSubplots()!");
    ImPlotSubplot& subplot = *gp.CurrentSubplot;
    const ImGuiIO& IO  = ImGui::GetIO();
    // set alignments
    for (int r = 0; r < subplot.Rows; ++r)
        subplot.RowAlignmentData[r].End();
    for (int c = 0; c < subplot.Cols; ++c)
        subplot.ColAlignmentData[c].End();
    // pop styling
    PopStyleColor();
    PopStyleVar();
    PopStyleVar();
    ImGui::PopStyleVar();
    // legend
    subplot.Items.Legend.Hovered = false;
    for (int i = 0; i < subplot.Items.GetItemCount(); ++i)
        subplot.Items.GetItemByIndex(i)->LegendHovered = false;
    // render legend
    const bool share_items = ImHasFlag(subplot.Flags, ImPlotSubplotFlags_ShareItems);
    ImDrawList& DrawList = *ImGui::GetWindowDrawList();
    if (share_items && !ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoLegend) && subplot.Items.GetLegendCount() > 0) {
        ImPlotLegend& legend = subplot.Items.Legend;
        const bool   legend_horz = ImHasFlag(legend.Flags, ImPlotLegendFlags_Horizontal);
        const ImVec2 legend_size = CalcLegendSize(subplot.Items, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, !legend_horz);
        const ImVec2 legend_pos  = GetLocationPos(subplot.FrameRect, legend_size, legend.Location, gp.Style.PlotPadding);
        legend.Rect = ImRect(legend_pos, legend_pos + legend_size);
        legend.RectClamped = legend.Rect;
        const bool legend_scrollable = ClampLegendRect(legend.RectClamped,subplot.FrameRect, gp.Style.PlotPadding);
        const ImGuiButtonFlags legend_button_flags = ImGuiButtonFlags_AllowOverlap
                                                    | ImGuiButtonFlags_PressedOnClick
                                                    | ImGuiButtonFlags_PressedOnDoubleClick
                                                    | ImGuiButtonFlags_MouseButtonLeft
                                                    | ImGuiButtonFlags_MouseButtonRight
                                                    | ImGuiButtonFlags_MouseButtonMiddle
                                                    | ImGuiButtonFlags_FlattenChildren;
        ImGui::KeepAliveID(subplot.Items.ID);
        ImGui::ButtonBehavior(legend.RectClamped, subplot.Items.ID, &legend.Hovered, &legend.Held, legend_button_flags);
        legend.Hovered = legend.Hovered || (subplot.FrameHovered && legend.RectClamped.Contains(ImGui::GetIO().MousePos));

        if (legend_scrollable) {
            if (legend.Hovered) {
                ImGui::SetKeyOwner(ImGuiKey_MouseWheelY, subplot.Items.ID);
                if (IO.MouseWheel != 0.0f) {
                    ImVec2 max_step = legend.Rect.GetSize() * 0.67f;
                    float font_size = ImGui::GetCurrentWindow()->CalcFontSize();
                    float scroll_step = ImFloor(ImMin(2 * font_size, max_step.x));
                    legend.Scroll.x += scroll_step * IO.MouseWheel;
                    legend.Scroll.y += scroll_step * IO.MouseWheel;
                }
            }
            const ImVec2 min_scroll_offset = legend.RectClamped.GetSize() - legend.Rect.GetSize();
            legend.Scroll.x = ImClamp(legend.Scroll.x, min_scroll_offset.x, 0.0f);
            legend.Scroll.y = ImClamp(legend.Scroll.y, min_scroll_offset.y, 0.0f);
            const ImVec2 scroll_offset = legend_horz ? ImVec2(legend.Scroll.x, 0) : ImVec2(0, legend.Scroll.y);
            ImVec2 legend_offset = legend.RectClamped.Min - legend.Rect.Min + scroll_offset;
            legend.Rect.Min += legend_offset;
            legend.Rect.Max += legend_offset;
        } else {
            legend.Scroll = ImVec2(0,0);
        }

        const ImU32 col_bg = GetStyleColorU32(ImPlotCol_LegendBg);
        const ImU32 col_bd = GetStyleColorU32(ImPlotCol_LegendBorder);
        ImGui::PushClipRect(legend.RectClamped.Min, legend.RectClamped.Max, true);
        DrawList.AddRectFilled(legend.RectClamped.Min, legend.RectClamped.Max, col_bg);
        bool legend_contextable = ShowLegendEntries(subplot.Items, legend.Rect, legend.Hovered, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, !legend_horz, DrawList)
                                && !ImHasFlag(legend.Flags, ImPlotLegendFlags_NoMenus);
        DrawList.AddRect(legend.RectClamped.Min, legend.RectClamped.Max, col_bd);
        ImGui::PopClipRect();

        if (legend_contextable && !ImHasFlag(subplot.Flags, ImPlotSubplotFlags_NoMenus) && ImGui::GetIO().MouseReleased[gp.InputMap.Menu])
            ImGui::OpenPopup("##LegendContext");
        if (ImGui::BeginPopup("##LegendContext")) {
            ImGui::Text("Legend"); ImGui::Separator();
            if (ShowLegendContextMenu(legend, !ImHasFlag(subplot.Flags, ImPlotFlags_NoLegend)))
                ImFlipFlag(subplot.Flags, ImPlotFlags_NoLegend);
            ImGui::EndPopup();
        }
    }
    else {
        subplot.Items.Legend.Rect = ImRect();
    }
    // remove items
    if (gp.CurrentItems == &subplot.Items)
        gp.CurrentItems = nullptr;
    // reset the plot items for the next frame (TODO: put this elswhere)
    for (int i = 0; i < subplot.Items.GetItemCount(); ++i) {
        subplot.Items.GetItemByIndex(i)->SeenThisFrame = false;
    }
    // pop id
    ImGui::PopID();
    // set DC back correctly
    GImGui->CurrentWindow->DC.CursorPos = subplot.FrameRect.Min;
    ImGui::Dummy(subplot.FrameRect.GetSize());
    ResetCtxForNextSubplot(GImPlot);

}

//-----------------------------------------------------------------------------
// [SECTION] Plot Utils
//-----------------------------------------------------------------------------

void SetAxis(ImAxis axis) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "SetAxis() needs to be called between BeginPlot() and EndPlot()!");
    IM_ASSERT_USER_ERROR(axis >= ImAxis_X1 && axis < ImAxis_COUNT, "Axis index out of bounds!");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot->Axes[axis].Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    SetupLock();
    if (axis < ImAxis_Y1)
        gp.CurrentPlot->CurrentX = axis;
    else
        gp.CurrentPlot->CurrentY = axis;
}

void SetAxes(ImAxis x_idx, ImAxis y_idx) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "SetAxes() needs to be called between BeginPlot() and EndPlot()!");
    IM_ASSERT_USER_ERROR(x_idx >= ImAxis_X1 && x_idx < ImAxis_Y1, "X-Axis index out of bounds!");
    IM_ASSERT_USER_ERROR(y_idx >= ImAxis_Y1 && y_idx < ImAxis_COUNT, "Y-Axis index out of bounds!");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot->Axes[x_idx].Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    IM_ASSERT_USER_ERROR(gp.CurrentPlot->Axes[y_idx].Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
    SetupLock();
    gp.CurrentPlot->CurrentX = x_idx;
    gp.CurrentPlot->CurrentY = y_idx;
}

ImPlotPoint PixelsToPlot(float x, float y, ImAxis x_idx, ImAxis y_idx) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "PixelsToPlot() needs to be called between BeginPlot() and EndPlot()!");
    IM_ASSERT_USER_ERROR(x_idx == IMPLOT_AUTO || (x_idx >= ImAxis_X1 && x_idx < ImAxis_Y1),    "X-Axis index out of bounds!");
    IM_ASSERT_USER_ERROR(y_idx == IMPLOT_AUTO || (y_idx >= ImAxis_Y1 && y_idx < ImAxis_COUNT), "Y-Axis index out of bounds!");
    SetupLock();
    ImPlotPlot& plot   = *gp.CurrentPlot;
    ImPlotAxis& x_axis = x_idx == IMPLOT_AUTO ? plot.Axes[plot.CurrentX] : plot.Axes[x_idx];
    ImPlotAxis& y_axis = y_idx == IMPLOT_AUTO ? plot.Axes[plot.CurrentY] : plot.Axes[y_idx];
    return ImPlotPoint( x_axis.PixelsToPlot(x), y_axis.PixelsToPlot(y) );
}

ImPlotPoint PixelsToPlot(const ImVec2& pix, ImAxis x_idx, ImAxis y_idx) {
    return PixelsToPlot(pix.x, pix.y, x_idx, y_idx);
}

ImVec2 PlotToPixels(double x, double y, ImAxis x_idx, ImAxis y_idx) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "PlotToPixels() needs to be called between BeginPlot() and EndPlot()!");
    IM_ASSERT_USER_ERROR(x_idx == IMPLOT_AUTO || (x_idx >= ImAxis_X1 && x_idx < ImAxis_Y1),    "X-Axis index out of bounds!");
    IM_ASSERT_USER_ERROR(y_idx == IMPLOT_AUTO || (y_idx >= ImAxis_Y1 && y_idx < ImAxis_COUNT), "Y-Axis index out of bounds!");
    SetupLock();
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& x_axis = x_idx == IMPLOT_AUTO ? plot.Axes[plot.CurrentX] : plot.Axes[x_idx];
    ImPlotAxis& y_axis = y_idx == IMPLOT_AUTO ? plot.Axes[plot.CurrentY] : plot.Axes[y_idx];
    return ImVec2( x_axis.PlotToPixels(x), y_axis.PlotToPixels(y) );
}

ImVec2 PlotToPixels(const ImPlotPoint& plt, ImAxis x_idx, ImAxis y_idx) {
    return PlotToPixels(plt.x, plt.y, x_idx, y_idx);
}

ImVec2 GetPlotPos() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "GetPlotPos() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    return gp.CurrentPlot->PlotRect.Min;
}

ImVec2 GetPlotSize() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "GetPlotSize() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    return gp.CurrentPlot->PlotRect.GetSize();
}

ImPlotPoint GetPlotMousePos(ImAxis x_idx, ImAxis y_idx) {
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != nullptr, "GetPlotMousePos() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    return PixelsToPlot(ImGui::GetMousePos(), x_idx, y_idx);
}

ImPlotRect GetPlotLimits(ImAxis x_idx, ImAxis y_idx) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "GetPlotLimits() needs to be called between BeginPlot() and EndPlot()!");
    IM_ASSERT_USER_ERROR(x_idx == IMPLOT_AUTO || (x_idx >= ImAxis_X1 && x_idx < ImAxis_Y1),    "X-Axis index out of bounds!");
    IM_ASSERT_USER_ERROR(y_idx == IMPLOT_AUTO || (y_idx >= ImAxis_Y1 && y_idx < ImAxis_COUNT), "Y-Axis index out of bounds!");
    SetupLock();
    ImPlotPlot& plot = *gp.CurrentPlot;
    ImPlotAxis& x_axis = x_idx == IMPLOT_AUTO ? plot.Axes[plot.CurrentX] : plot.Axes[x_idx];
    ImPlotAxis& y_axis = y_idx == IMPLOT_AUTO ? plot.Axes[plot.CurrentY] : plot.Axes[y_idx];
    ImPlotRect limits;
    limits.X = x_axis.Range;
    limits.Y = y_axis.Range;
    return limits;
}

bool IsPlotHovered() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "IsPlotHovered() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    return gp.CurrentPlot->Hovered;
}

bool IsAxisHovered(ImAxis axis) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "IsPlotXAxisHovered() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    return gp.CurrentPlot->Axes[axis].Hovered;
}

bool IsSubplotsHovered() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentSubplot != nullptr, "IsSubplotsHovered() needs to be called between BeginSubplots() and EndSubplots()!");
    return gp.CurrentSubplot->FrameHovered;
}

bool IsPlotSelected() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "IsPlotSelected() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    return gp.CurrentPlot->Selected;
}

ImPlotRect GetPlotSelection(ImAxis x_idx, ImAxis y_idx) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "GetPlotSelection() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    ImPlotPlot& plot = *gp.CurrentPlot;
    if (!plot.Selected)
        return ImPlotRect(0,0,0,0);
    ImPlotPoint p1 = PixelsToPlot(plot.SelectRect.Min + plot.PlotRect.Min, x_idx, y_idx);
    ImPlotPoint p2 = PixelsToPlot(plot.SelectRect.Max + plot.PlotRect.Min, x_idx, y_idx);
    ImPlotRect result;
    result.X.Min = ImMin(p1.x, p2.x);
    result.X.Max = ImMax(p1.x, p2.x);
    result.Y.Min = ImMin(p1.y, p2.y);
    result.Y.Max = ImMax(p1.y, p2.y);
    return result;
}

void CancelPlotSelection() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "CancelPlotSelection() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    ImPlotPlot& plot = *gp.CurrentPlot;
    if (plot.Selected)
        plot.Selected = plot.Selecting = false;
}

void HideNextItem(bool hidden, ImPlotCond cond) {
    ImPlotContext& gp = *GImPlot;
    gp.NextItemData.HasHidden  = true;
    gp.NextItemData.Hidden     = hidden;
    gp.NextItemData.HiddenCond = cond;
}

//-----------------------------------------------------------------------------
// [SECTION] Plot Tools
//-----------------------------------------------------------------------------

void Annotation(double x, double y, const ImVec4& col, const ImVec2& offset, bool clamp, bool round) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "Annotation() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    char x_buff[IMPLOT_LABEL_MAX_SIZE];
    char y_buff[IMPLOT_LABEL_MAX_SIZE];
    ImPlotAxis& x_axis = gp.CurrentPlot->Axes[gp.CurrentPlot->CurrentX];
    ImPlotAxis& y_axis = gp.CurrentPlot->Axes[gp.CurrentPlot->CurrentY];
    LabelAxisValue(x_axis, x, x_buff, sizeof(x_buff), round);
    LabelAxisValue(y_axis, y, y_buff, sizeof(y_buff), round);
    Annotation(x,y,col,offset,clamp,"%s, %s",x_buff,y_buff);
}

void AnnotationV(double x, double y, const ImVec4& col, const ImVec2& offset, bool clamp, const char* fmt, va_list args) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "Annotation() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    ImVec2 pos = PlotToPixels(x,y,IMPLOT_AUTO,IMPLOT_AUTO);
    ImU32  bg  = ImGui::GetColorU32(col);
    ImU32  fg  = col.w == 0 ? GetStyleColorU32(ImPlotCol_InlayText) : CalcTextColor(col);
    gp.Annotations.AppendV(pos, offset, bg, fg, clamp, fmt, args);
}

void Annotation(double x, double y, const ImVec4& col, const ImVec2& offset, bool clamp, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AnnotationV(x,y,col,offset,clamp,fmt,args);
    va_end(args);
}

void TagV(ImAxis axis, double v, const ImVec4& col, const char* fmt, va_list args) {
    ImPlotContext& gp = *GImPlot;
    SetupLock();
    ImU32 bg = ImGui::GetColorU32(col);
    ImU32 fg = col.w == 0 ? GetStyleColorU32(ImPlotCol_AxisText) : CalcTextColor(col);
    gp.Tags.AppendV(axis,v,bg,fg,fmt,args);
}

void Tag(ImAxis axis, double v, const ImVec4& col, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    TagV(axis,v,col,fmt,args);
    va_end(args);
}

void Tag(ImAxis axis, double v, const ImVec4& color, bool round) {
    ImPlotContext& gp = *GImPlot;
    SetupLock();
    char buff[IMPLOT_LABEL_MAX_SIZE];
    ImPlotAxis& ax = gp.CurrentPlot->Axes[axis];
    LabelAxisValue(ax, v, buff, sizeof(buff), round);
    Tag(axis,v,color,"%s",buff);
}

IMPLOT_API void TagX(double x, const ImVec4& color, bool round) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "TagX() needs to be called between BeginPlot() and EndPlot()!");
    Tag(gp.CurrentPlot->CurrentX, x, color, round);
}

IMPLOT_API void TagX(double x, const ImVec4& color, const char* fmt, ...) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "TagX() needs to be called between BeginPlot() and EndPlot()!");
    va_list args;
    va_start(args, fmt);
    TagV(gp.CurrentPlot->CurrentX,x,color,fmt,args);
    va_end(args);
}

IMPLOT_API void TagXV(double x, const ImVec4& color, const char* fmt, va_list args) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "TagX() needs to be called between BeginPlot() and EndPlot()!");
    TagV(gp.CurrentPlot->CurrentX, x, color, fmt, args);
}

IMPLOT_API void TagY(double y, const ImVec4& color, bool round) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "TagY() needs to be called between BeginPlot() and EndPlot()!");
    Tag(gp.CurrentPlot->CurrentY, y, color, round);
}

IMPLOT_API void TagY(double y, const ImVec4& color, const char* fmt, ...) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "TagY() needs to be called between BeginPlot() and EndPlot()!");
    va_list args;
    va_start(args, fmt);
    TagV(gp.CurrentPlot->CurrentY,y,color,fmt,args);
    va_end(args);
}

IMPLOT_API void TagYV(double y, const ImVec4& color, const char* fmt, va_list args) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "TagY() needs to be called between BeginPlot() and EndPlot()!");
    TagV(gp.CurrentPlot->CurrentY, y, color, fmt, args);
}

static const float DRAG_GRAB_HALF_SIZE = 4.0f;

bool DragPoint(int n_id, double* x, double* y, const ImVec4& col, float radius, ImPlotDragToolFlags flags, bool* out_clicked, bool* out_hovered, bool* out_held) {
    ImGui::PushID("#IMPLOT_DRAG_POINT");
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != nullptr, "DragPoint() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();

    if (!ImHasFlag(flags,ImPlotDragToolFlags_NoFit) && FitThisFrame()) {
        FitPoint(ImPlotPoint(*x,*y));
    }

    const bool input = !ImHasFlag(flags, ImPlotDragToolFlags_NoInputs);
    const bool show_curs = !ImHasFlag(flags, ImPlotDragToolFlags_NoCursors);
    const bool no_delay = !ImHasFlag(flags, ImPlotDragToolFlags_Delayed);
    const float grab_half_size = ImMax(DRAG_GRAB_HALF_SIZE, radius);
    const ImVec4 color = IsColorAuto(col) ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : col;
    const ImU32 col32 = ImGui::ColorConvertFloat4ToU32(color);

    ImVec2 pos = PlotToPixels(*x,*y,IMPLOT_AUTO,IMPLOT_AUTO);
    const ImGuiID id = ImGui::GetCurrentWindow()->GetID(n_id);
    ImRect rect(pos.x-grab_half_size,pos.y-grab_half_size,pos.x+grab_half_size,pos.y+grab_half_size);
    bool hovered = false, held = false;

    ImGui::KeepAliveID(id);
    if (input) {
        bool clicked = ImGui::ButtonBehavior(rect,id,&hovered,&held);
        if (out_clicked) *out_clicked = clicked;
        if (out_hovered) *out_hovered = hovered;
        if (out_held)    *out_held    = held;
    }

    bool modified = false;
    if (held && ImGui::IsMouseDragging(0)) {
        *x = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).x;
        *y = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).y;
        modified = true;
    }

    PushPlotClipRect();
    ImDrawList& DrawList = *GetPlotDrawList();
    if ((hovered || held) && show_curs)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (modified && no_delay)
        pos = PlotToPixels(*x,*y,IMPLOT_AUTO,IMPLOT_AUTO);
    DrawList.AddCircleFilled(pos, radius, col32);
    PopPlotClipRect();

    ImGui::PopID();
    return modified;
}

bool DragLineX(int n_id, double* value, const ImVec4& col, float thickness, ImPlotDragToolFlags flags, bool* out_clicked, bool* out_hovered, bool* out_held) {
    // ImGui::PushID("#IMPLOT_DRAG_LINE_X");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "DragLineX() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();

    if (!ImHasFlag(flags,ImPlotDragToolFlags_NoFit) && FitThisFrame()) {
        FitPointX(*value);
    }

    const bool input = !ImHasFlag(flags, ImPlotDragToolFlags_NoInputs);
    const bool show_curs = !ImHasFlag(flags, ImPlotDragToolFlags_NoCursors);
    const bool no_delay = !ImHasFlag(flags, ImPlotDragToolFlags_Delayed);
    const float grab_half_size = ImMax(DRAG_GRAB_HALF_SIZE, thickness/2);
    float yt = gp.CurrentPlot->PlotRect.Min.y;
    float yb = gp.CurrentPlot->PlotRect.Max.y;
    float x  = IM_ROUND(PlotToPixels(*value,0,IMPLOT_AUTO,IMPLOT_AUTO).x);
    const ImGuiID id = ImGui::GetCurrentWindow()->GetID(n_id);
    ImRect rect(x-grab_half_size,yt,x+grab_half_size,yb);
    bool hovered = false, held = false;

    ImGui::KeepAliveID(id);
    if (input) {
        bool clicked = ImGui::ButtonBehavior(rect,id,&hovered,&held);
        if (out_clicked) *out_clicked = clicked;
        if (out_hovered) *out_hovered = hovered;
        if (out_held)    *out_held    = held;
    }

    if ((hovered || held) && show_curs)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

    float len = gp.Style.MajorTickLen.x;
    ImVec4 color = IsColorAuto(col) ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : col;
    ImU32 col32 = ImGui::ColorConvertFloat4ToU32(color);

    bool modified = false;
    if (held && ImGui::IsMouseDragging(0)) {
        *value = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).x;
        modified = true;
    }

    PushPlotClipRect();
    ImDrawList& DrawList = *GetPlotDrawList();
    if (modified && no_delay)
        x  = IM_ROUND(PlotToPixels(*value,0,IMPLOT_AUTO,IMPLOT_AUTO).x);
    DrawList.AddLine(ImVec2(x,yt), ImVec2(x,yb),     col32,   thickness);
    DrawList.AddLine(ImVec2(x,yt), ImVec2(x,yt+len), col32, 3*thickness);
    DrawList.AddLine(ImVec2(x,yb), ImVec2(x,yb-len), col32, 3*thickness);
    PopPlotClipRect();

    // ImGui::PopID();
    return modified;
}

bool DragLineY(int n_id, double* value, const ImVec4& col, float thickness, ImPlotDragToolFlags flags, bool* out_clicked, bool* out_hovered, bool* out_held) {
    ImGui::PushID("#IMPLOT_DRAG_LINE_Y");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "DragLineY() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();

    if (!ImHasFlag(flags,ImPlotDragToolFlags_NoFit) && FitThisFrame()) {
        FitPointY(*value);
    }

    const bool input = !ImHasFlag(flags, ImPlotDragToolFlags_NoInputs);
    const bool show_curs = !ImHasFlag(flags, ImPlotDragToolFlags_NoCursors);
    const bool no_delay = !ImHasFlag(flags, ImPlotDragToolFlags_Delayed);
    const float grab_half_size = ImMax(DRAG_GRAB_HALF_SIZE, thickness/2);
    float xl = gp.CurrentPlot->PlotRect.Min.x;
    float xr = gp.CurrentPlot->PlotRect.Max.x;
    float y  = IM_ROUND(PlotToPixels(0, *value,IMPLOT_AUTO,IMPLOT_AUTO).y);

    const ImGuiID id = ImGui::GetCurrentWindow()->GetID(n_id);
    ImRect rect(xl,y-grab_half_size,xr,y+grab_half_size);
    bool hovered = false, held = false;

    ImGui::KeepAliveID(id);
    if (input) {
        bool clicked = ImGui::ButtonBehavior(rect,id,&hovered,&held);
        if (out_clicked) *out_clicked = clicked;
        if (out_hovered) *out_hovered = hovered;
        if (out_held)    *out_held    = held;
    }

    if ((hovered || held) && show_curs)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

    float len = gp.Style.MajorTickLen.y;
    ImVec4 color = IsColorAuto(col) ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : col;
    ImU32 col32 = ImGui::ColorConvertFloat4ToU32(color);

    bool modified = false;
    if (held && ImGui::IsMouseDragging(0)) {
        *value = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).y;
        modified = true;
    }

    PushPlotClipRect();
    ImDrawList& DrawList = *GetPlotDrawList();
    if (modified && no_delay)
        y  = IM_ROUND(PlotToPixels(0, *value,IMPLOT_AUTO,IMPLOT_AUTO).y);
    DrawList.AddLine(ImVec2(xl,y), ImVec2(xr,y),     col32,   thickness);
    DrawList.AddLine(ImVec2(xl,y), ImVec2(xl+len,y), col32, 3*thickness);
    DrawList.AddLine(ImVec2(xr,y), ImVec2(xr-len,y), col32, 3*thickness);
    PopPlotClipRect();

    ImGui::PopID();
    return modified;
}

bool DragRect(int n_id, double* x_min, double* y_min, double* x_max, double* y_max, const ImVec4& col, ImPlotDragToolFlags flags, bool* out_clicked, bool* out_hovered, bool* out_held) {
    ImGui::PushID("#IMPLOT_DRAG_RECT");
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != nullptr, "DragRect() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();

    if (!ImHasFlag(flags,ImPlotDragToolFlags_NoFit) && FitThisFrame()) {
        FitPoint(ImPlotPoint(*x_min,*y_min));
        FitPoint(ImPlotPoint(*x_max,*y_max));
    }

    const bool input = !ImHasFlag(flags, ImPlotDragToolFlags_NoInputs);
    const bool show_curs = !ImHasFlag(flags, ImPlotDragToolFlags_NoCursors);
    const bool no_delay = !ImHasFlag(flags, ImPlotDragToolFlags_Delayed);
    bool    h[] = {true,false,true,false};
    double* x[] = {x_min,x_max,x_max,x_min};
    double* y[] = {y_min,y_min,y_max,y_max};
    ImVec2 p[4];
    for (int i = 0; i < 4; ++i)
        p[i] = PlotToPixels(*x[i],*y[i],IMPLOT_AUTO,IMPLOT_AUTO);
    ImVec2 pc = PlotToPixels((*x_min+*x_max)/2,(*y_min+*y_max)/2,IMPLOT_AUTO,IMPLOT_AUTO);
    ImRect rect(ImMin(p[0],p[2]),ImMax(p[0],p[2]));
    ImRect rect_grab = rect; rect_grab.Expand(DRAG_GRAB_HALF_SIZE);

    ImGuiMouseCursor cur[4];
    if (show_curs) {
        cur[0] = (rect.Min.x == p[0].x && rect.Min.y == p[0].y) || (rect.Max.x == p[0].x && rect.Max.y == p[0].y) ? ImGuiMouseCursor_ResizeNWSE : ImGuiMouseCursor_ResizeNESW;
        cur[1] = cur[0] == ImGuiMouseCursor_ResizeNWSE ? ImGuiMouseCursor_ResizeNESW : ImGuiMouseCursor_ResizeNWSE;
        cur[2] = cur[1] == ImGuiMouseCursor_ResizeNWSE ? ImGuiMouseCursor_ResizeNESW : ImGuiMouseCursor_ResizeNWSE;
        cur[3] = cur[2] == ImGuiMouseCursor_ResizeNWSE ? ImGuiMouseCursor_ResizeNESW : ImGuiMouseCursor_ResizeNWSE;
    }

    ImVec4 color = IsColorAuto(col) ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : col;
    ImU32 col32 = ImGui::ColorConvertFloat4ToU32(color);
    color.w *= 0.25f;
    ImU32 col32_a = ImGui::ColorConvertFloat4ToU32(color);
    const ImGuiID id = ImGui::GetCurrentWindow()->GetID(n_id);

    bool modified = false;
    bool clicked = false, hovered = false, held = false;
    ImRect b_rect(pc.x-DRAG_GRAB_HALF_SIZE,pc.y-DRAG_GRAB_HALF_SIZE,pc.x+DRAG_GRAB_HALF_SIZE,pc.y+DRAG_GRAB_HALF_SIZE);

    ImGui::KeepAliveID(id);
    if (input) {
        // middle point
        clicked = ImGui::ButtonBehavior(b_rect,id,&hovered,&held);
        if (out_clicked) *out_clicked = clicked;
        if (out_hovered) *out_hovered = hovered;
        if (out_held)    *out_held    = held;
    }

    if ((hovered || held) && show_curs)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    if (held && ImGui::IsMouseDragging(0)) {
        for (int i = 0; i < 4; ++i) {
            ImPlotPoint pp = PixelsToPlot(p[i] + ImGui::GetIO().MouseDelta,IMPLOT_AUTO,IMPLOT_AUTO);
            *y[i] = pp.y;
            *x[i] = pp.x;
        }
        modified = true;
    }

    for (int i = 0; i < 4; ++i) {
        // points
        b_rect = ImRect(p[i].x-DRAG_GRAB_HALF_SIZE,p[i].y-DRAG_GRAB_HALF_SIZE,p[i].x+DRAG_GRAB_HALF_SIZE,p[i].y+DRAG_GRAB_HALF_SIZE);
        ImGuiID p_id = id + i + 1;
        ImGui::KeepAliveID(p_id);
        if (input) {
            clicked = ImGui::ButtonBehavior(b_rect,p_id,&hovered,&held);
            if (out_clicked) *out_clicked = *out_clicked || clicked;
            if (out_hovered) *out_hovered = *out_hovered || hovered;
            if (out_held)    *out_held    = *out_held    || held;
        }
        if ((hovered || held) && show_curs)
            ImGui::SetMouseCursor(cur[i]);

        if (held && ImGui::IsMouseDragging(0)) {
            *x[i] = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).x;
            *y[i] = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).y;
            modified = true;
        }

        // edges
        ImVec2 e_min = ImMin(p[i],p[(i+1)%4]);
        ImVec2 e_max = ImMax(p[i],p[(i+1)%4]);
        b_rect = h[i] ? ImRect(e_min.x + DRAG_GRAB_HALF_SIZE, e_min.y - DRAG_GRAB_HALF_SIZE, e_max.x - DRAG_GRAB_HALF_SIZE, e_max.y + DRAG_GRAB_HALF_SIZE)
                    : ImRect(e_min.x - DRAG_GRAB_HALF_SIZE, e_min.y + DRAG_GRAB_HALF_SIZE, e_max.x + DRAG_GRAB_HALF_SIZE, e_max.y - DRAG_GRAB_HALF_SIZE);
        ImGuiID e_id = id + i + 5;
        ImGui::KeepAliveID(e_id);
        if (input) {
            clicked = ImGui::ButtonBehavior(b_rect,e_id,&hovered,&held);
            if (out_clicked) *out_clicked = *out_clicked || clicked;
            if (out_hovered) *out_hovered = *out_hovered || hovered;
            if (out_held)    *out_held    = *out_held    || held;
        }
        if ((hovered || held) && show_curs)
            h[i] ? ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS) : ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        if (held && ImGui::IsMouseDragging(0)) {
            if (h[i])
                *y[i] = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).y;
            else
                *x[i] = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).x;
            modified = true;
        }
        if (hovered && ImGui::IsMouseDoubleClicked(0))
        {
            ImPlotRect b = GetPlotLimits(IMPLOT_AUTO,IMPLOT_AUTO);
            if (h[i])
                *y[i] = ((y[i] == y_min && *y_min < *y_max) || (y[i] == y_max && *y_max < *y_min)) ? b.Y.Min : b.Y.Max;
            else
                *x[i] = ((x[i] == x_min && *x_min < *x_max) || (x[i] == x_max && *x_max < *x_min)) ? b.X.Min : b.X.Max;
            modified = true;
        }
    }

    const bool mouse_inside = rect_grab.Contains(ImGui::GetMousePos());
    const bool mouse_clicked = ImGui::IsMouseClicked(0);
    const bool mouse_down = ImGui::IsMouseDown(0);
    if (input && mouse_inside) {
        if (out_clicked) *out_clicked = *out_clicked || mouse_clicked;
        if (out_hovered) *out_hovered = true;
        if (out_held)    *out_held    = *out_held    || mouse_down;
    }

    PushPlotClipRect();
    ImDrawList& DrawList = *GetPlotDrawList();
    if (modified && no_delay) {
        for (int i = 0; i < 4; ++i)
            p[i] = PlotToPixels(*x[i],*y[i],IMPLOT_AUTO,IMPLOT_AUTO);
        pc = PlotToPixels((*x_min+*x_max)/2,(*y_min+*y_max)/2,IMPLOT_AUTO,IMPLOT_AUTO);
        rect = ImRect(ImMin(p[0],p[2]),ImMax(p[0],p[2]));
    }
    DrawList.AddRectFilled(rect.Min, rect.Max, col32_a);
    DrawList.AddRect(rect.Min, rect.Max, col32);
    if (input && (modified || mouse_inside)) {
        DrawList.AddCircleFilled(pc,DRAG_GRAB_HALF_SIZE,col32);
        for (int i = 0; i < 4; ++i)
            DrawList.AddCircleFilled(p[i],DRAG_GRAB_HALF_SIZE,col32);
    }
    PopPlotClipRect();
    ImGui::PopID();
    return modified;
}

bool DragRect(int id, ImPlotRect* bounds, const ImVec4& col, ImPlotDragToolFlags flags, bool* out_clicked, bool* out_hovered, bool* out_held) {
    return DragRect(id, &bounds->X.Min, &bounds->Y.Min,&bounds->X.Max, &bounds->Y.Max, col, flags, out_clicked, out_hovered, out_held);
}

//-----------------------------------------------------------------------------
// [SECTION] Legend Utils and Tools
//-----------------------------------------------------------------------------

bool IsLegendEntryHovered(const char* label_id) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentItems != nullptr, "IsPlotItemHighlight() needs to be called within an itemized context!");
    SetupLock();
    ImGuiID id = ImGui::GetIDWithSeed(label_id, nullptr, gp.CurrentItems->ID);
    ImPlotItem* item = gp.CurrentItems->GetItem(id);
    return item && item->LegendHovered;
}

bool BeginLegendPopup(const char* label_id, ImGuiMouseButton mouse_button) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentItems != nullptr, "BeginLegendPopup() needs to be called within an itemized context!");
    SetupLock();
    ImGuiWindow* window = GImGui->CurrentWindow;
    if (window->SkipItems)
        return false;
    ImGuiID id = ImGui::GetIDWithSeed(label_id, nullptr, gp.CurrentItems->ID);
    if (ImGui::IsMouseReleased(mouse_button)) {
        ImPlotItem* item = gp.CurrentItems->GetItem(id);
        if (item && item->LegendHovered)
            ImGui::OpenPopupEx(id);
    }
    return ImGui::BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
}

void EndLegendPopup() {
    SetupLock();
    ImGui::EndPopup();
}

void ShowAltLegend(const char* title_id, bool vertical, const ImVec2 size, bool interactable) {
    ImPlotContext& gp    = *GImPlot;
    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return;
    ImDrawList &DrawList = *Window->DrawList;
    ImPlotPlot* plot = GetPlot(title_id);
    ImVec2 legend_size;
    ImVec2 default_size = gp.Style.LegendPadding * 2;
    if (plot != nullptr) {
        legend_size  = CalcLegendSize(plot->Items, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, vertical);
        default_size = legend_size + gp.Style.LegendPadding * 2;
    }
    ImVec2 frame_size = ImGui::CalcItemSize(size, default_size.x, default_size.y);
    ImRect bb_frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ImGui::ItemSize(bb_frame);
    if (!ImGui::ItemAdd(bb_frame, 0, &bb_frame))
        return;
    ImGui::RenderFrame(bb_frame.Min, bb_frame.Max, GetStyleColorU32(ImPlotCol_FrameBg), true, G.Style.FrameRounding);
    DrawList.PushClipRect(bb_frame.Min, bb_frame.Max, true);
    if (plot != nullptr) {
        const ImVec2 legend_pos  = GetLocationPos(bb_frame, legend_size, 0, gp.Style.LegendPadding);
        const ImRect legend_bb(legend_pos, legend_pos + legend_size);
        interactable = interactable && bb_frame.Contains(ImGui::GetIO().MousePos);
        // render legend box
        ImU32  col_bg      = GetStyleColorU32(ImPlotCol_LegendBg);
        ImU32  col_bd      = GetStyleColorU32(ImPlotCol_LegendBorder);
        DrawList.AddRectFilled(legend_bb.Min, legend_bb.Max, col_bg);
        DrawList.AddRect(legend_bb.Min, legend_bb.Max, col_bd);
        // render entries
        ShowLegendEntries(plot->Items, legend_bb, interactable, gp.Style.LegendInnerPadding, gp.Style.LegendSpacing, vertical, DrawList);
    }
    DrawList.PopClipRect();
}

//-----------------------------------------------------------------------------
// [SECTION] Drag and Drop Utils
//-----------------------------------------------------------------------------

bool BeginDragDropTargetPlot() {
    SetupLock();
    ImPlotContext& gp = *GImPlot;
    ImRect rect = gp.CurrentPlot->PlotRect;
    return ImGui::BeginDragDropTargetCustom(rect, gp.CurrentPlot->ID);
}

bool BeginDragDropTargetAxis(ImAxis axis) {
    SetupLock();
    ImPlotPlot& plot = *GImPlot->CurrentPlot;
    ImPlotAxis& ax = plot.Axes[axis];
    ImRect rect = ax.HoverRect;
    rect.Expand(-3.5f);
    return ImGui::BeginDragDropTargetCustom(rect, ax.ID);
}

bool BeginDragDropTargetLegend() {
    SetupLock();
    ImPlotItemGroup& items = *GImPlot->CurrentItems;
    ImRect rect = items.Legend.RectClamped;
    return ImGui::BeginDragDropTargetCustom(rect, items.ID);
}

void EndDragDropTarget() {
    SetupLock();
    ImGui::EndDragDropTarget();
}

bool BeginDragDropSourcePlot(ImGuiDragDropFlags flags) {
    SetupLock();
    ImPlotContext& gp = *GImPlot;
    ImPlotPlot* plot = gp.CurrentPlot;
    if (GImGui->IO.KeyMods == gp.InputMap.OverrideMod || GImGui->DragDropPayload.SourceId == plot->ID)
        return ImGui::ItemAdd(plot->PlotRect, plot->ID) && ImGui::BeginDragDropSource(flags);
    return false;
}

bool BeginDragDropSourceAxis(ImAxis idx, ImGuiDragDropFlags flags) {
    SetupLock();
    ImPlotContext& gp = *GImPlot;
    ImPlotAxis& axis = gp.CurrentPlot->Axes[idx];
    if (GImGui->IO.KeyMods == gp.InputMap.OverrideMod || GImGui->DragDropPayload.SourceId == axis.ID)
        return ImGui::ItemAdd(axis.HoverRect, axis.ID) && ImGui::BeginDragDropSource(flags);
    return false;
}

bool BeginDragDropSourceItem(const char* label_id, ImGuiDragDropFlags flags) {
    SetupLock();
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentItems != nullptr, "BeginDragDropSourceItem() needs to be called within an itemized context!");
    ImGuiID item_id = ImGui::GetIDWithSeed(label_id, nullptr, gp.CurrentItems->ID);
    ImPlotItem* item = gp.CurrentItems->GetItem(item_id);
    if (item != nullptr) {
        return ImGui::ItemAdd(item->LegendHoverRect, item->ID) && ImGui::BeginDragDropSource(flags);
    }
    return false;
}

void EndDragDropSource() {
    SetupLock();
    ImGui::EndDragDropSource();
}

//-----------------------------------------------------------------------------
// [SECTION] Aligned Plots
//-----------------------------------------------------------------------------

bool BeginAlignedPlots(const char* group_id, bool vertical) {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentAlignmentH == nullptr && gp.CurrentAlignmentV == nullptr, "Mismatched BeginAlignedPlots()/EndAlignedPlots()!");
    ImGuiContext &G = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    const ImGuiID ID = Window->GetID(group_id);
    ImPlotAlignmentData* alignment = gp.AlignmentData.GetOrAddByKey(ID);
    if (vertical)
        gp.CurrentAlignmentV = alignment;
    else
        gp.CurrentAlignmentH = alignment;
    if (alignment->Vertical != vertical)
        alignment->Reset();
    alignment->Vertical = vertical;
    alignment->Begin();
    return true;
}

void EndAlignedPlots() {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentAlignmentH != nullptr || gp.CurrentAlignmentV != nullptr, "Mismatched BeginAlignedPlots()/EndAlignedPlots()!");
    ImPlotAlignmentData* alignment = gp.CurrentAlignmentH != nullptr ? gp.CurrentAlignmentH : (gp.CurrentAlignmentV != nullptr ? gp.CurrentAlignmentV : nullptr);
    if (alignment)
        alignment->End();
    ResetCtxForNextAlignedPlots(GImPlot);
}

//-----------------------------------------------------------------------------
// [SECTION] Plot and Item Styling
//-----------------------------------------------------------------------------

ImPlotStyle& GetStyle() {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    return gp.Style;
}

void PushStyleColor(ImPlotCol idx, ImU32 col) {
    ImPlotContext& gp = *GImPlot;
    ImGuiColorMod backup;
    backup.Col = (ImGuiCol)idx;
    backup.BackupValue = gp.Style.Colors[idx];
    gp.ColorModifiers.push_back(backup);
    gp.Style.Colors[idx] = ImGui::ColorConvertU32ToFloat4(col);
}

void PushStyleColor(ImPlotCol idx, const ImVec4& col) {
    ImPlotContext& gp = *GImPlot;
    ImGuiColorMod backup;
    backup.Col = (ImGuiCol)idx;
    backup.BackupValue = gp.Style.Colors[idx];
    gp.ColorModifiers.push_back(backup);
    gp.Style.Colors[idx] = col;
}

void PopStyleColor(int count) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(count <= gp.ColorModifiers.Size, "You can't pop more modifiers than have been pushed!");
    while (count > 0)
    {
        ImGuiColorMod& backup = gp.ColorModifiers.back();
        gp.Style.Colors[backup.Col] = backup.BackupValue;
        gp.ColorModifiers.pop_back();
        count--;
    }
}

void PushStyleVar(ImPlotStyleVar idx, float val) {
    ImPlotContext& gp = *GImPlot;
    const ImPlotStyleVarInfo* var_info = GetPlotStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1) {
        float* pvar = (float*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod((ImGuiStyleVar)idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() float variant but variable is not a float!");
}

void PushStyleVar(ImPlotStyleVar idx, int val) {
    ImPlotContext& gp = *GImPlot;
    const ImPlotStyleVarInfo* var_info = GetPlotStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_S32 && var_info->Count == 1) {
        int* pvar = (int*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod((ImGuiStyleVar)idx, *pvar));
        *pvar = val;
        return;
    }
    else if (var_info->Type == ImGuiDataType_Float && var_info->Count == 1) {
        float* pvar = (float*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod((ImGuiStyleVar)idx, *pvar));
        *pvar = (float)val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() int variant but variable is not a int!");
}

void PushStyleVar(ImPlotStyleVar idx, const ImVec2& val)
{
    ImPlotContext& gp = *GImPlot;
    const ImPlotStyleVarInfo* var_info = GetPlotStyleVarInfo(idx);
    if (var_info->Type == ImGuiDataType_Float && var_info->Count == 2)
    {
        ImVec2* pvar = (ImVec2*)var_info->GetVarPtr(&gp.Style);
        gp.StyleModifiers.push_back(ImGuiStyleMod((ImGuiStyleVar)idx, *pvar));
        *pvar = val;
        return;
    }
    IM_ASSERT(0 && "Called PushStyleVar() ImVec2 variant but variable is not a ImVec2!");
}

void PopStyleVar(int count) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(count <= gp.StyleModifiers.Size, "You can't pop more modifiers than have been pushed!");
    while (count > 0) {
        ImGuiStyleMod& backup = gp.StyleModifiers.back();
        const ImPlotStyleVarInfo* info = GetPlotStyleVarInfo(backup.VarIdx);
        void* data = info->GetVarPtr(&gp.Style);
        if (info->Type == ImGuiDataType_Float && info->Count == 1) {
            ((float*)data)[0] = backup.BackupFloat[0];
        }
        else if (info->Type == ImGuiDataType_Float && info->Count == 2) {
             ((float*)data)[0] = backup.BackupFloat[0];
             ((float*)data)[1] = backup.BackupFloat[1];
        }
        else if (info->Type == ImGuiDataType_S32 && info->Count == 1) {
            ((int*)data)[0] = backup.BackupInt[0];
        }
        gp.StyleModifiers.pop_back();
        count--;
    }
}

//------------------------------------------------------------------------------
// [Section] Colormaps
//------------------------------------------------------------------------------

ImPlotColormap AddColormap(const char* name, const ImVec4* colormap, int size, bool qual) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(size > 1, "The colormap size must be greater than 1!");
    IM_ASSERT_USER_ERROR(gp.ColormapData.GetIndex(name) == -1, "The colormap name has already been used!");
    ImVector<ImU32> buffer;
    buffer.resize(size);
    for (int i = 0; i < size; ++i)
        buffer[i] = ImGui::ColorConvertFloat4ToU32(colormap[i]);
    return gp.ColormapData.Append(name, buffer.Data, size, qual);
}

ImPlotColormap AddColormap(const char* name, const ImU32*  colormap, int size, bool qual) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(size > 1, "The colormap size must be greater than 1!");
    IM_ASSERT_USER_ERROR(gp.ColormapData.GetIndex(name) == -1, "The colormap name has already be used!");
    return gp.ColormapData.Append(name, colormap, size, qual);
}

int GetColormapCount() {
    ImPlotContext& gp = *GImPlot;
    return gp.ColormapData.Count;
}

const char* GetColormapName(ImPlotColormap colormap) {
    ImPlotContext& gp = *GImPlot;
    return gp.ColormapData.GetName(colormap);
}

ImPlotColormap GetColormapIndex(const char* name) {
    ImPlotContext& gp = *GImPlot;
    return gp.ColormapData.GetIndex(name);
}

void PushColormap(ImPlotColormap colormap) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(colormap >= 0 && colormap < gp.ColormapData.Count, "The colormap index is invalid!");
    gp.ColormapModifiers.push_back(gp.Style.Colormap);
    gp.Style.Colormap = colormap;
}

void PushColormap(const char* name) {
    ImPlotContext& gp = *GImPlot;
    ImPlotColormap idx = gp.ColormapData.GetIndex(name);
    IM_ASSERT_USER_ERROR(idx != -1, "The colormap name is invalid!");
    PushColormap(idx);
}

void PopColormap(int count) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(count <= gp.ColormapModifiers.Size, "You can't pop more modifiers than have been pushed!");
    while (count > 0) {
        const ImPlotColormap& backup = gp.ColormapModifiers.back();
        gp.Style.Colormap     = backup;
        gp.ColormapModifiers.pop_back();
        count--;
    }
}

ImU32 NextColormapColorU32() {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentItems != nullptr, "NextColormapColor() needs to be called between BeginPlot() and EndPlot()!");
    int idx = gp.CurrentItems->ColormapIdx % gp.ColormapData.GetKeyCount(gp.Style.Colormap);
    ImU32 col  = gp.ColormapData.GetKeyColor(gp.Style.Colormap, idx);
    gp.CurrentItems->ColormapIdx++;
    return col;
}

ImVec4 NextColormapColor() {
    return ImGui::ColorConvertU32ToFloat4(NextColormapColorU32());
}

int GetColormapSize(ImPlotColormap cmap) {
    ImPlotContext& gp = *GImPlot;
    cmap = cmap == IMPLOT_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");
    return gp.ColormapData.GetKeyCount(cmap);
}

ImU32 GetColormapColorU32(int idx, ImPlotColormap cmap) {
    ImPlotContext& gp = *GImPlot;
    cmap = cmap == IMPLOT_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");
    idx = idx % gp.ColormapData.GetKeyCount(cmap);
    return gp.ColormapData.GetKeyColor(cmap, idx);
}

ImVec4 GetColormapColor(int idx, ImPlotColormap cmap) {
    return ImGui::ColorConvertU32ToFloat4(GetColormapColorU32(idx,cmap));
}

ImU32  SampleColormapU32(float t, ImPlotColormap cmap) {
    ImPlotContext& gp = *GImPlot;
    cmap = cmap == IMPLOT_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");
    return gp.ColormapData.LerpTable(cmap, t);
}

ImVec4 SampleColormap(float t, ImPlotColormap cmap) {
    return ImGui::ColorConvertU32ToFloat4(SampleColormapU32(t,cmap));
}

void RenderColorBar(const ImU32* colors, int size, ImDrawList& DrawList, const ImRect& bounds, bool vert, bool reversed, bool continuous) {
    const int n = continuous ? size - 1 : size;
    ImU32 col1, col2;
    if (vert) {
        const float step = bounds.GetHeight() / n;
        ImRect rect(bounds.Min.x, bounds.Min.y, bounds.Max.x, bounds.Min.y + step);
        for (int i = 0; i < n; ++i) {
            if (reversed) {
                col1 = colors[size-i-1];
                col2 = continuous ? colors[size-i-2] : col1;
            }
            else {
                col1 = colors[i];
                col2 = continuous ? colors[i+1] : col1;
            }
            DrawList.AddRectFilledMultiColor(rect.Min, rect.Max, col1, col1, col2, col2);
            rect.TranslateY(step);
        }
    }
    else {
        const float step = bounds.GetWidth() / n;
        ImRect rect(bounds.Min.x, bounds.Min.y, bounds.Min.x + step, bounds.Max.y);
        for (int i = 0; i < n; ++i) {
            if (reversed) {
                col1 = colors[size-i-1];
                col2 = continuous ? colors[size-i-2] : col1;
            }
            else {
                col1 = colors[i];
                col2 = continuous ? colors[i+1] : col1;
            }
            DrawList.AddRectFilledMultiColor(rect.Min, rect.Max, col1, col2, col2, col1);
            rect.TranslateX(step);
        }
    }
}

void ColormapScale(const char* label, double scale_min, double scale_max, const ImVec2& size, const char* format, ImPlotColormapScaleFlags flags, ImPlotColormap cmap) {
    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return;

    const ImGuiID ID = Window->GetID(label);
    ImVec2 label_size(0,0);
    if (!ImHasFlag(flags, ImPlotColormapScaleFlags_NoLabel)) {
        label_size = ImGui::CalcTextSize(label,nullptr,true);
    }

    ImPlotContext& gp = *GImPlot;
    cmap = cmap == IMPLOT_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");

    ImVec2 frame_size  = ImGui::CalcItemSize(size, 0, gp.Style.PlotDefaultSize.y);
    if (frame_size.y < gp.Style.PlotMinSize.y && size.y < 0.0f)
        frame_size.y = gp.Style.PlotMinSize.y;

    ImPlotRange range(ImMin(scale_min,scale_max), ImMax(scale_min,scale_max));
    gp.CTicker.Reset();
    Locator_Default(gp.CTicker, range, frame_size.y, true, Formatter_Default, (void*)format);

    const bool rend_label = label_size.x > 0;
    const float txt_off   = gp.Style.LabelPadding.x;
    const float pad       = txt_off + gp.CTicker.MaxSize.x + (rend_label ? txt_off + label_size.y : 0);
    float bar_w           = 20;
    if (frame_size.x == 0)
        frame_size.x = bar_w + pad + 2 * gp.Style.PlotPadding.x;
    else {
        bar_w = frame_size.x - (pad + 2 * gp.Style.PlotPadding.x);
        if (bar_w < gp.Style.MajorTickLen.y)
            bar_w = gp.Style.MajorTickLen.y;
    }

    ImDrawList &DrawList = *Window->DrawList;
    ImRect bb_frame = ImRect(Window->DC.CursorPos, Window->DC.CursorPos + frame_size);
    ImGui::ItemSize(bb_frame);
    if (!ImGui::ItemAdd(bb_frame, ID, &bb_frame))
        return;

    ImGui::RenderFrame(bb_frame.Min, bb_frame.Max, GetStyleColorU32(ImPlotCol_FrameBg), true, G.Style.FrameRounding);

    const bool opposite = ImHasFlag(flags, ImPlotColormapScaleFlags_Opposite);
    const bool inverted = ImHasFlag(flags, ImPlotColormapScaleFlags_Invert);
    const bool reversed = scale_min > scale_max;

    float bb_grad_shift = opposite ? pad : 0;
    ImRect bb_grad(bb_frame.Min + gp.Style.PlotPadding + ImVec2(bb_grad_shift, 0),
                   bb_frame.Min + ImVec2(bar_w + gp.Style.PlotPadding.x + bb_grad_shift,
                                         frame_size.y - gp.Style.PlotPadding.y));

    ImGui::PushClipRect(bb_frame.Min, bb_frame.Max, true);
    const ImU32 col_text = ImGui::GetColorU32(ImGuiCol_Text);

    const bool invert_scale = inverted ? (reversed ? false : true) : (reversed ? true : false);
    const float y_min = invert_scale ? bb_grad.Max.y : bb_grad.Min.y;
    const float y_max = invert_scale ? bb_grad.Min.y : bb_grad.Max.y;

    RenderColorBar(gp.ColormapData.GetKeys(cmap), gp.ColormapData.GetKeyCount(cmap), DrawList, bb_grad, true, !inverted, !gp.ColormapData.IsQual(cmap));
    for (int i = 0; i < gp.CTicker.TickCount(); ++i) {
        const double y_pos_plt = gp.CTicker.Ticks[i].PlotPos;
        const float y_pos = ImRemap((float)y_pos_plt, (float)range.Max, (float)range.Min, y_min, y_max);
        const float tick_width = gp.CTicker.Ticks[i].Major ? gp.Style.MajorTickLen.y : gp.Style.MinorTickLen.y;
        const float tick_thick = gp.CTicker.Ticks[i].Major ? gp.Style.MajorTickSize.y : gp.Style.MinorTickSize.y;
        const float tick_t     = (float)((y_pos_plt - scale_min) / (scale_max - scale_min));
        const ImU32 tick_col = CalcTextColor(gp.ColormapData.LerpTable(cmap,tick_t));
        if (y_pos < bb_grad.Max.y - 2 && y_pos > bb_grad.Min.y + 2) {
            DrawList.AddLine(opposite ? ImVec2(bb_grad.Min.x+1, y_pos) : ImVec2(bb_grad.Max.x-1, y_pos),
                             opposite ? ImVec2(bb_grad.Min.x + tick_width, y_pos) : ImVec2(bb_grad.Max.x - tick_width, y_pos),
                             tick_col,
                             tick_thick);
        }
        const float txt_x = opposite ? bb_grad.Min.x - txt_off - gp.CTicker.Ticks[i].LabelSize.x : bb_grad.Max.x + txt_off;
        const float txt_y = y_pos - gp.CTicker.Ticks[i].LabelSize.y * 0.5f;
        DrawList.AddText(ImVec2(txt_x, txt_y), col_text, gp.CTicker.GetText(i));
    }

    if (rend_label) {
        const float pos_x = opposite ? bb_frame.Min.x + gp.Style.PlotPadding.x : bb_grad.Max.x + 2 * txt_off + gp.CTicker.MaxSize.x;
        const float pos_y = bb_grad.GetCenter().y + label_size.x * 0.5f;
        const char* label_end = ImGui::FindRenderedTextEnd(label);
        AddTextVertical(&DrawList,ImVec2(pos_x,pos_y),col_text,label,label_end);
    }
    DrawList.AddRect(bb_grad.Min, bb_grad.Max, GetStyleColorU32(ImPlotCol_PlotBorder));
    ImGui::PopClipRect();
}

bool ColormapSlider(const char* label, float* t, ImVec4* out, const char* format, ImPlotColormap cmap) {
    *t = ImClamp(*t,0.0f,1.0f);
    ImGuiContext &G      = *GImGui;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    ImPlotContext& gp = *GImPlot;
    cmap = cmap == IMPLOT_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");
    const ImU32* keys  = gp.ColormapData.GetKeys(cmap);
    const int    count = gp.ColormapData.GetKeyCount(cmap);
    const bool   qual  = gp.ColormapData.IsQual(cmap);
    const ImVec2 pos  = ImGui::GetCurrentWindow()->DC.CursorPos;
    const float w     = ImGui::CalcItemWidth();
    const float h     = ImGui::GetFrameHeight();
    const ImRect rect = ImRect(pos.x,pos.y,pos.x+w,pos.y+h);
    RenderColorBar(keys,count,*ImGui::GetWindowDrawList(),rect,false,false,!qual);
    const ImU32 grab = CalcTextColor(gp.ColormapData.LerpTable(cmap,*t));
    // const ImU32 text = CalcTextColor(gp.ColormapData.LerpTable(cmap,0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,IM_COL32_BLACK_TRANS);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,IM_COL32_BLACK_TRANS);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,ImVec4(1,1,1,0.1f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,grab);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, grab);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize,2);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,0);
    const bool changed = ImGui::SliderFloat(label,t,0,1,format);
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(2);
    if (out != nullptr)
        *out = ImGui::ColorConvertU32ToFloat4(gp.ColormapData.LerpTable(cmap,*t));
    return changed;
}

bool ColormapButton(const char* label, const ImVec2& size_arg, ImPlotColormap cmap) {
    ImGuiContext &G      = *GImGui;
    const ImGuiStyle& style = G.Style;
    ImGuiWindow * Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    ImPlotContext& gp = *GImPlot;
    cmap = cmap == IMPLOT_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");
    const ImU32* keys  = gp.ColormapData.GetKeys(cmap);
    const int    count = gp.ColormapData.GetKeyCount(cmap);
    const bool   qual  = gp.ColormapData.IsQual(cmap);
    const ImVec2 pos  = ImGui::GetCurrentWindow()->DC.CursorPos;
    const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
    const ImRect rect = ImRect(pos.x,pos.y,pos.x+size.x,pos.y+size.y);
    RenderColorBar(keys,count,*ImGui::GetWindowDrawList(),rect,false,false,!qual);
    const ImU32 text = CalcTextColor(gp.ColormapData.LerpTable(cmap,G.Style.ButtonTextAlign.x));
    ImGui::PushStyleColor(ImGuiCol_Button,IM_COL32_BLACK_TRANS);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(1,1,1,0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(1,1,1,0.2f));
    ImGui::PushStyleColor(ImGuiCol_Text,text);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,0);
    const bool pressed = ImGui::Button(label,size);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    return pressed;
}

//-----------------------------------------------------------------------------
// [Section] Miscellaneous
//-----------------------------------------------------------------------------

ImPlotInputMap& GetInputMap() {
    IM_ASSERT_USER_ERROR(GImPlot != nullptr, "No current context. Did you call ImPlot::CreateContext() or ImPlot::SetCurrentContext()?");
    ImPlotContext& gp = *GImPlot;
    return gp.InputMap;
}

void MapInputDefault(ImPlotInputMap* dst) {
    ImPlotInputMap& map = dst ? *dst : GetInputMap();
    map.Pan             = ImGuiMouseButton_Left;
    map.PanMod          = ImGuiMod_None;
    map.Fit             = ImGuiMouseButton_Left;
    map.Menu            = ImGuiMouseButton_Right;
    map.Select          = ImGuiMouseButton_Right;
    map.SelectMod       = ImGuiMod_None;
    map.SelectCancel    = ImGuiMouseButton_Left;
    map.SelectHorzMod   = ImGuiMod_Alt;
    map.SelectVertMod   = ImGuiMod_Shift;
    map.OverrideMod     = ImGuiMod_Ctrl;
    map.ZoomMod         = ImGuiMod_None;
    map.ZoomRate        = 0.1f;
}

void MapInputReverse(ImPlotInputMap* dst) {
    ImPlotInputMap& map = dst ? *dst : GetInputMap();
    map.Pan             = ImGuiMouseButton_Right;
    map.PanMod          = ImGuiMod_None;
    map.Fit             = ImGuiMouseButton_Left;
    map.Menu            = ImGuiMouseButton_Right;
    map.Select          = ImGuiMouseButton_Left;
    map.SelectMod       = ImGuiMod_None;
    map.SelectCancel    = ImGuiMouseButton_Right;
    map.SelectHorzMod   = ImGuiMod_Alt;
    map.SelectVertMod   = ImGuiMod_Shift;
    map.OverrideMod     = ImGuiMod_Ctrl;
    map.ZoomMod         = ImGuiMod_None;
    map.ZoomRate        = 0.1f;
}

//-----------------------------------------------------------------------------
// [Section] Miscellaneous
//-----------------------------------------------------------------------------

void ItemIcon(const ImVec4& col) {
    ItemIcon(ImGui::ColorConvertFloat4ToU32(col));
}

void ItemIcon(ImU32 col) {
    const float txt_size = ImGui::GetTextLineHeight();
    ImVec2 size(txt_size-4,txt_size);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImVec2 pos = window->DC.CursorPos;
    ImGui::GetWindowDrawList()->AddRectFilled(pos + ImVec2(0,2), pos + size - ImVec2(0,2), col);
    ImGui::Dummy(size);
}

void ColormapIcon(ImPlotColormap cmap) {
    ImPlotContext& gp = *GImPlot;
    const float txt_size = ImGui::GetTextLineHeight();
    ImVec2 size(txt_size-4,txt_size);
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImVec2 pos = window->DC.CursorPos;
    ImRect rect(pos+ImVec2(0,2),pos+size-ImVec2(0,2));
    ImDrawList& DrawList = *ImGui::GetWindowDrawList();
    RenderColorBar(gp.ColormapData.GetKeys(cmap),gp.ColormapData.GetKeyCount(cmap),DrawList,rect,false,false,!gp.ColormapData.IsQual(cmap));
    ImGui::Dummy(size);
}

ImDrawList* GetPlotDrawList() {
    return ImGui::GetWindowDrawList();
}

void PushPlotClipRect(float expand) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "PushPlotClipRect() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    ImRect rect = gp.CurrentPlot->PlotRect;
    rect.Expand(expand);
    ImGui::PushClipRect(rect.Min, rect.Max, true);
}

void PopPlotClipRect() {
    SetupLock();
    ImGui::PopClipRect();
}

static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

bool ShowStyleSelector(const char* label)
{
    static int style_idx = -1;
    if (ImGui::Combo(label, &style_idx, "Auto\0Classic\0Dark\0Light\0"))
    {
        switch (style_idx)
        {
        case 0: StyleColorsAuto(); break;
        case 1: StyleColorsClassic(); break;
        case 2: StyleColorsDark(); break;
        case 3: StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

bool ShowColormapSelector(const char* label) {
    ImPlotContext& gp = *GImPlot;
    bool set = false;
    if (ImGui::BeginCombo(label, gp.ColormapData.GetName(gp.Style.Colormap))) {
        for (int i = 0; i < gp.ColormapData.Count; ++i) {
            const char* name = gp.ColormapData.GetName(i);
            if (ImGui::Selectable(name, gp.Style.Colormap == i)) {
                gp.Style.Colormap = i;
                ImPlot::BustItemCache();
                set = true;
            }
        }
        ImGui::EndCombo();
    }
    return set;
}

bool ShowInputMapSelector(const char* label) {
    static int map_idx = -1;
    if (ImGui::Combo(label, &map_idx, "Default\0Reversed\0"))
    {
        switch (map_idx)
        {
        case 0: MapInputDefault(); break;
        case 1: MapInputReverse(); break;
        }
        return true;
    }
    return false;
}


void ShowStyleEditor(ImPlotStyle* ref) {
    ImPlotContext& gp = *GImPlot;
    ImPlotStyle& style = GetStyle();
    static ImPlotStyle ref_saved_style;
    // Default to using internal storage as reference
    static bool init = true;
    if (init && ref == nullptr)
        ref_saved_style = style;
    init = false;
    if (ref == nullptr)
        ref = &ref_saved_style;

    if (ImPlot::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
               "Use \"Export\" below to save them somewhere.");
    if (ImGui::BeginTabBar("##StyleEditor")) {
        if (ImGui::BeginTabItem("Variables")) {
            ImGui::Text("Item Styling");
            ImGui::SliderFloat("LineWeight", &style.LineWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("MarkerSize", &style.MarkerSize, 2.0f, 10.0f, "%.1f");
            ImGui::SliderFloat("MarkerWeight", &style.MarkerWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("FillAlpha", &style.FillAlpha, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat("ErrorBarSize", &style.ErrorBarSize, 0.0f, 10.0f, "%.1f");
            ImGui::SliderFloat("ErrorBarWeight", &style.ErrorBarWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("DigitalBitHeight", &style.DigitalBitHeight, 0.0f, 20.0f, "%.1f");
            ImGui::SliderFloat("DigitalBitGap", &style.DigitalBitGap, 0.0f, 20.0f, "%.1f");
            ImGui::Text("Plot Styling");
            ImGui::SliderFloat("PlotBorderSize", &style.PlotBorderSize, 0.0f, 2.0f, "%.0f");
            ImGui::SliderFloat("MinorAlpha", &style.MinorAlpha, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat2("MajorTickLen", (float*)&style.MajorTickLen, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("MinorTickLen", (float*)&style.MinorTickLen, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("MajorTickSize",  (float*)&style.MajorTickSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("MinorTickSize", (float*)&style.MinorTickSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("MajorGridSize", (float*)&style.MajorGridSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("MinorGridSize", (float*)&style.MinorGridSize, 0.0f, 2.0f, "%.1f");
            ImGui::SliderFloat2("PlotDefaultSize", (float*)&style.PlotDefaultSize, 0.0f, 1000, "%.0f");
            ImGui::SliderFloat2("PlotMinSize", (float*)&style.PlotMinSize, 0.0f, 300, "%.0f");
            ImGui::Text("Plot Padding");
            ImGui::SliderFloat2("PlotPadding", (float*)&style.PlotPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LabelPadding", (float*)&style.LabelPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LegendPadding", (float*)&style.LegendPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LegendInnerPadding", (float*)&style.LegendInnerPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat2("LegendSpacing", (float*)&style.LegendSpacing, 0.0f, 5.0f, "%.0f");
            ImGui::SliderFloat2("MousePosPadding", (float*)&style.MousePosPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("AnnotationPadding", (float*)&style.AnnotationPadding, 0.0f, 5.0f, "%.0f");
            ImGui::SliderFloat2("FitPadding", (float*)&style.FitPadding, 0, 0.2f, "%.2f");

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Colors")) {
            static int output_dest = 0;
            static bool output_only_modified = false;

            if (ImGui::Button("Export", ImVec2(75,0))) {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImPlot::GetStyle().Colors;\n");
                for (int i = 0; i < ImPlotCol_COUNT; i++) {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImPlot::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0) {
                        if (IsColorAuto(i))
                            ImGui::LogText("colors[ImPlotCol_%s]%*s= IMPLOT_AUTO_COL;\n",name,14 - (int)strlen(name), "");
                        else
                            ImGui::LogText("colors[ImPlotCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n",
                                        name, 14 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
                    }
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            HelpMarker(
                "In the color list:\n"
                "Left-click on colored square to open color picker,\n"
                "Right-click to open edit options menu.");
            ImGui::Separator();
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImPlotCol_COUNT; i++) {
                const char* name = ImPlot::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImVec4 temp = GetStyleColorVec4(i);
                const bool is_auto = IsColorAuto(i);
                if (!is_auto)
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
                if (ImGui::Button("Auto")) {
                    if (is_auto)
                        style.Colors[i] = temp;
                    else
                        style.Colors[i] = IMPLOT_AUTO_COL;
                    BustItemCache();
                }
                if (!is_auto)
                    ImGui::PopStyleVar();
                ImGui::SameLine();
                if (ImGui::ColorEdit4(name, &temp.x, ImGuiColorEditFlags_NoInputs | alpha_flags)) {
                    style.Colors[i] = temp;
                    BustItemCache();
                }
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0) {
                    ImGui::SameLine(175); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                    ImGui::SameLine(); if (ImGui::Button("Revert")) {
                        style.Colors[i] = ref->Colors[i];
                        BustItemCache();
                    }
                }
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::Separator();
            ImGui::Text("Colors that are set to Auto (i.e. IMPLOT_AUTO_COL) will\n"
                        "be automatically deduced from your ImGui style or the\n"
                        "current ImPlot Colormap. If you want to style individual\n"
                        "plot items, use Push/PopStyleColor around its function.");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Colormaps")) {
            static int output_dest = 0;
            if (ImGui::Button("Export", ImVec2(75,0))) {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                int size = GetColormapSize();
                const char* name = GetColormapName(gp.Style.Colormap);
                ImGui::LogText("static const ImU32 %s_Data[%d] = {\n", name, size);
                for (int i = 0; i < size; ++i) {
                    ImU32 col = GetColormapColorU32(i,gp.Style.Colormap);
                    ImGui::LogText("    %u%s\n", col, i == size - 1 ? "" : ",");
                }
                ImGui::LogText("};\nImPlotColormap %s = ImPlot::AddColormap(\"%s\", %s_Data, %d);", name, name, name, size);
                ImGui::LogFinish();
            }
            ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine();
            static bool edit = false;
            ImGui::Checkbox("Edit Mode",&edit);

            // built-in/added
            ImGui::Separator();
            for (int i = 0; i < gp.ColormapData.Count; ++i) {
                ImGui::PushID(i);
                int size = gp.ColormapData.GetKeyCount(i);
                bool selected = i == gp.Style.Colormap;

                const char* name = GetColormapName(i);
                if (!selected)
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
                if (ImGui::Button(name, ImVec2(100,0))) {
                    gp.Style.Colormap = i;
                    BustItemCache();
                }
                if (!selected)
                    ImGui::PopStyleVar();
                ImGui::SameLine();
                ImGui::BeginGroup();
                if (edit) {
                    for (int c = 0; c < size; ++c) {
                        ImGui::PushID(c);
                        ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(gp.ColormapData.GetKeyColor(i,c));
                        if (ImGui::ColorEdit4("",&col4.x,ImGuiColorEditFlags_NoInputs)) {
                            ImU32 col32 = ImGui::ColorConvertFloat4ToU32(col4);
                            gp.ColormapData.SetKeyColor(i,c,col32);
                            BustItemCache();
                        }
                        if ((c + 1) % 12 != 0 && c != size -1)
                            ImGui::SameLine();
                        ImGui::PopID();
                    }
                }
                else {
                    if (ImPlot::ColormapButton("##",ImVec2(-1,0),i))
                        edit = true;
                }
                ImGui::EndGroup();
                ImGui::PopID();
            }


            static ImVector<ImVec4> custom;
            if (custom.Size == 0) {
                custom.push_back(ImVec4(1,0,0,1));
                custom.push_back(ImVec4(0,1,0,1));
                custom.push_back(ImVec4(0,0,1,1));
            }
            ImGui::Separator();
            ImGui::BeginGroup();
            static char name[16] = "MyColormap";


            if (ImGui::Button("+", ImVec2((100 - ImGui::GetStyle().ItemSpacing.x)/2,0)))
                custom.push_back(ImVec4(0,0,0,1));
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2((100 - ImGui::GetStyle().ItemSpacing.x)/2,0)) && custom.Size > 2)
                custom.pop_back();
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##Name",name,16,ImGuiInputTextFlags_CharsNoBlank);
            static bool qual = true;
            ImGui::Checkbox("Qualitative",&qual);
            if (ImGui::Button("Add", ImVec2(100, 0)) && gp.ColormapData.GetIndex(name)==-1)
                AddColormap(name,custom.Data,custom.Size,qual);

            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            for (int c = 0; c < custom.Size; ++c) {
                ImGui::PushID(c);
                if (ImGui::ColorEdit4("##Col1", &custom[c].x, ImGuiColorEditFlags_NoInputs)) {

                }
                if ((c + 1) % 12 != 0)
                    ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::EndGroup();


            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ShowUserGuide() {
        ImGui::BulletText("Left-click drag within the plot area to pan X and Y axes.");
    ImGui::Indent();
        ImGui::BulletText("Left-click drag on axis labels to pan an individual axis.");
    ImGui::Unindent();
    ImGui::BulletText("Scroll in the plot area to zoom both X any Y axes.");
    ImGui::Indent();
        ImGui::BulletText("Scroll on axis labels to zoom an individual axis.");
    ImGui::Unindent();
    ImGui::BulletText("Right-click drag to box select data.");
    ImGui::Indent();
        ImGui::BulletText("Hold Alt to expand box selection horizontally.");
        ImGui::BulletText("Hold Shift to expand box selection vertically.");
        ImGui::BulletText("Left-click while box selecting to cancel the selection.");
    ImGui::Unindent();
    ImGui::BulletText("Double left-click to fit all visible data.");
    ImGui::Indent();
        ImGui::BulletText("Double left-click axis labels to fit the individual axis.");
    ImGui::Unindent();
    ImGui::BulletText("Right-click open the full plot context menu.");
    ImGui::Indent();
        ImGui::BulletText("Right-click axis labels to open an individual axis context menu.");
    ImGui::Unindent();
    ImGui::BulletText("Click legend label icons to show/hide plot items.");
}

void ShowTicksMetrics(const ImPlotTicker& ticker) {
    ImGui::BulletText("Size: %d", ticker.TickCount());
    ImGui::BulletText("MaxSize: [%f,%f]", ticker.MaxSize.x, ticker.MaxSize.y);
}

void ShowAxisMetrics(const ImPlotPlot& plot, const ImPlotAxis& axis) {
    ImGui::BulletText("Label: %s", axis.LabelOffset == -1 ? "[none]" : plot.GetAxisLabel(axis));
    ImGui::BulletText("Flags: 0x%08X", axis.Flags);
    ImGui::BulletText("Range: [%f,%f]",axis.Range.Min, axis.Range.Max);
    ImGui::BulletText("Pixels: %f", axis.PixelSize());
    ImGui::BulletText("Aspect: %f", axis.GetAspect());
    ImGui::BulletText(axis.OrthoAxis == nullptr ? "OrtherAxis: NULL" : "OrthoAxis: 0x%08X", axis.OrthoAxis->ID);
    ImGui::BulletText("LinkedMin: %p", (void*)axis.LinkedMin);
    ImGui::BulletText("LinkedMax: %p", (void*)axis.LinkedMax);
    ImGui::BulletText("HasRange: %s", axis.HasRange ? "true" : "false");
    ImGui::BulletText("Hovered: %s", axis.Hovered ? "true" : "false");
    ImGui::BulletText("Held: %s", axis.Held ? "true" : "false");

    if (ImGui::TreeNode("Transform")) {
        ImGui::BulletText("PixelMin: %f", axis.PixelMin);
        ImGui::BulletText("PixelMax: %f", axis.PixelMax);
        ImGui::BulletText("ScaleToPixel: %f", axis.ScaleToPixel);
        ImGui::BulletText("ScaleMax: %f", axis.ScaleMax);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Ticks")) {
        ShowTicksMetrics(axis.Ticker);
        ImGui::TreePop();
    }
}

void ShowMetricsWindow(bool* p_popen) {

    static bool show_plot_rects = false;
    static bool show_axes_rects = false;
    static bool show_axis_rects = false;
    static bool show_canvas_rects = false;
    static bool show_frame_rects = false;
    static bool show_subplot_frame_rects = false;
    static bool show_subplot_grid_rects = false;
    static bool show_legend_rects = false;

    ImDrawList& fg = *ImGui::GetForegroundDrawList();

    ImPlotContext& gp = *GImPlot;
    // ImGuiContext& g = *GImGui;
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("ImPlot Metrics", p_popen);
    ImGui::Text("ImPlot " IMPLOT_VERSION);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Text("Mouse Position: [%.0f,%.0f]", io.MousePos.x, io.MousePos.y);
    ImGui::Separator();
    if (ImGui::TreeNode("Tools")) {
        if (ImGui::Button("Bust Plot Cache"))
            BustPlotCache();
        ImGui::SameLine();
        if (ImGui::Button("Bust Item Cache"))
            BustItemCache();
        ImGui::Checkbox("Show Frame Rects", &show_frame_rects);
        ImGui::Checkbox("Show Canvas Rects",&show_canvas_rects);
        ImGui::Checkbox("Show Plot Rects",  &show_plot_rects);
        ImGui::Checkbox("Show Axes Rects",  &show_axes_rects);
        ImGui::Checkbox("Show Axis Rects",  &show_axis_rects);
        ImGui::Checkbox("Show Subplot Frame Rects",  &show_subplot_frame_rects);
        ImGui::Checkbox("Show Subplot Grid Rects",  &show_subplot_grid_rects);
        ImGui::Checkbox("Show Legend Rects",  &show_legend_rects);
        ImGui::TreePop();
    }
    const int n_plots = gp.Plots.GetBufSize();
    const int n_subplots = gp.Subplots.GetBufSize();
    // render rects
    for (int p = 0; p < n_plots; ++p) {
        ImPlotPlot* plot = gp.Plots.GetByIndex(p);
        if (show_frame_rects)
            fg.AddRect(plot->FrameRect.Min, plot->FrameRect.Max, IM_COL32(255,0,255,255));
        if (show_canvas_rects)
            fg.AddRect(plot->CanvasRect.Min, plot->CanvasRect.Max, IM_COL32(0,255,255,255));
        if (show_plot_rects)
            fg.AddRect(plot->PlotRect.Min, plot->PlotRect.Max, IM_COL32(255,255,0,255));
        if (show_axes_rects)
            fg.AddRect(plot->AxesRect.Min, plot->AxesRect.Max, IM_COL32(0,255,128,255));
        if (show_axis_rects) {
            for (int i = 0; i < ImAxis_COUNT; ++i) {
                if (plot->Axes[i].Enabled)
                    fg.AddRect(plot->Axes[i].HoverRect.Min, plot->Axes[i].HoverRect.Max, IM_COL32(0,255,0,255));
            }
        }
        if (show_legend_rects && plot->Items.GetLegendCount() > 0) {
            fg.AddRect(plot->Items.Legend.Rect.Min, plot->Items.Legend.Rect.Max, IM_COL32(255,192,0,255));
            fg.AddRect(plot->Items.Legend.RectClamped.Min, plot->Items.Legend.RectClamped.Max, IM_COL32(255,128,0,255));
        }
    }
    for (int p = 0; p < n_subplots; ++p) {
        ImPlotSubplot* subplot = gp.Subplots.GetByIndex(p);
        if (show_subplot_frame_rects)
            fg.AddRect(subplot->FrameRect.Min, subplot->FrameRect.Max, IM_COL32(255,0,0,255));
        if (show_subplot_grid_rects)
            fg.AddRect(subplot->GridRect.Min, subplot->GridRect.Max, IM_COL32(0,0,255,255));
        if (show_legend_rects && subplot->Items.GetLegendCount() > 0) {
            fg.AddRect(subplot->Items.Legend.Rect.Min, subplot->Items.Legend.Rect.Max, IM_COL32(255,192,0,255));
            fg.AddRect(subplot->Items.Legend.RectClamped.Min, subplot->Items.Legend.RectClamped.Max, IM_COL32(255,128,0,255));
        }
    }
    if (ImGui::TreeNode("Plots","Plots (%d)", n_plots)) {
        for (int p = 0; p < n_plots; ++p) {
            // plot
            ImPlotPlot& plot = *gp.Plots.GetByIndex(p);
            ImGui::PushID(p);
            if (ImGui::TreeNode("Plot", "Plot [0x%08X]", plot.ID)) {
                int n_items = plot.Items.GetItemCount();
                if (ImGui::TreeNode("Items", "Items (%d)", n_items)) {
                    for (int i = 0; i < n_items; ++i) {
                        ImPlotItem* item = plot.Items.GetItemByIndex(i);
                        ImGui::PushID(i);
                        if (ImGui::TreeNode("Item", "Item [0x%08X]", item->ID)) {
                            ImGui::Bullet(); ImGui::Checkbox("Show", &item->Show);
                            ImGui::Bullet();
                            ImVec4 temp = ImGui::ColorConvertU32ToFloat4(item->Color);
                            if (ImGui::ColorEdit4("Color",&temp.x, ImGuiColorEditFlags_NoInputs))
                                item->Color = ImGui::ColorConvertFloat4ToU32(temp);

                            ImGui::BulletText("NameOffset: %d",item->NameOffset);
                            ImGui::BulletText("Name: %s", item->NameOffset != -1 ? plot.Items.Legend.Labels.Buf.Data + item->NameOffset : "N/A");
                            ImGui::BulletText("Hovered: %s",item->LegendHovered ? "true" : "false");
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }
                char buff[16];
                for (int i = 0; i < IMPLOT_NUM_X_AXES; ++i) {
                    ImFormatString(buff,16,"X-Axis %d", i+1);
                    if (plot.XAxis(i).Enabled && ImGui::TreeNode(buff, "X-Axis %d [0x%08X]", i+1, plot.XAxis(i).ID)) {
                        ShowAxisMetrics(plot, plot.XAxis(i));
                        ImGui::TreePop();
                    }
                }
                for (int i = 0; i < IMPLOT_NUM_Y_AXES; ++i) {
                    ImFormatString(buff,16,"Y-Axis %d", i+1);
                    if (plot.YAxis(i).Enabled && ImGui::TreeNode(buff, "Y-Axis %d [0x%08X]", i+1, plot.YAxis(i).ID)) {
                        ShowAxisMetrics(plot, plot.YAxis(i));
                        ImGui::TreePop();
                    }
                }
                ImGui::BulletText("Title: %s", plot.HasTitle() ? plot.GetTitle() : "none");
                ImGui::BulletText("Flags: 0x%08X", plot.Flags);
                ImGui::BulletText("Initialized: %s", plot.Initialized ? "true" : "false");
                ImGui::BulletText("Selecting: %s", plot.Selecting ? "true" : "false");
                ImGui::BulletText("Selected: %s", plot.Selected ? "true" : "false");
                ImGui::BulletText("Hovered: %s", plot.Hovered ? "true" : "false");
                ImGui::BulletText("Held: %s", plot.Held ? "true" : "false");
                ImGui::BulletText("LegendHovered: %s", plot.Items.Legend.Hovered ? "true" : "false");
                ImGui::BulletText("ContextLocked: %s", plot.ContextLocked ? "true" : "false");
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Subplots","Subplots (%d)", n_subplots)) {
        for (int p = 0; p < n_subplots; ++p) {
            // plot
            ImPlotSubplot& plot = *gp.Subplots.GetByIndex(p);
            ImGui::PushID(p);
            if (ImGui::TreeNode("Subplot", "Subplot [0x%08X]", plot.ID)) {
                int n_items = plot.Items.GetItemCount();
                if (ImGui::TreeNode("Items", "Items (%d)", n_items)) {
                    for (int i = 0; i < n_items; ++i) {
                        ImPlotItem* item = plot.Items.GetItemByIndex(i);
                        ImGui::PushID(i);
                        if (ImGui::TreeNode("Item", "Item [0x%08X]", item->ID)) {
                            ImGui::Bullet(); ImGui::Checkbox("Show", &item->Show);
                            ImGui::Bullet();
                            ImVec4 temp = ImGui::ColorConvertU32ToFloat4(item->Color);
                            if (ImGui::ColorEdit4("Color",&temp.x, ImGuiColorEditFlags_NoInputs))
                                item->Color = ImGui::ColorConvertFloat4ToU32(temp);

                            ImGui::BulletText("NameOffset: %d",item->NameOffset);
                            ImGui::BulletText("Name: %s", item->NameOffset != -1 ? plot.Items.Legend.Labels.Buf.Data + item->NameOffset : "N/A");
                            ImGui::BulletText("Hovered: %s",item->LegendHovered ? "true" : "false");
                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }
                    ImGui::TreePop();
                }
                ImGui::BulletText("Flags: 0x%08X", plot.Flags);
                ImGui::BulletText("FrameHovered: %s", plot.FrameHovered ? "true" : "false");
                ImGui::BulletText("LegendHovered: %s", plot.Items.Legend.Hovered ? "true" : "false");
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Colormaps")) {
        ImGui::BulletText("Colormaps:  %d", gp.ColormapData.Count);
        ImGui::BulletText("Memory: %d bytes", gp.ColormapData.Tables.Size * 4);
        if (ImGui::TreeNode("Data")) {
            for (int m = 0; m < gp.ColormapData.Count; ++m) {
                if (ImGui::TreeNode(gp.ColormapData.GetName(m))) {
                    int count = gp.ColormapData.GetKeyCount(m);
                    int size = gp.ColormapData.GetTableSize(m);
                    bool qual = gp.ColormapData.IsQual(m);
                    ImGui::BulletText("Qualitative: %s", qual ? "true" : "false");
                    ImGui::BulletText("Key Count: %d", count);
                    ImGui::BulletText("Table Size: %d", size);
                    ImGui::Indent();

                    static float t = 0.5;
                    ImVec4 samp;
                    float wid = 32 * 10 - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemSpacing.x;
                    ImGui::SetNextItemWidth(wid);
                    ImPlot::ColormapSlider("##Sample",&t,&samp,"%.3f",m);
                    ImGui::SameLine();
                    ImGui::ColorButton("Sampler",samp);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0));
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
                    for (int c = 0; c < size; ++c) {
                        ImVec4 col = ImGui::ColorConvertU32ToFloat4(gp.ColormapData.GetTableColor(m,c));
                        ImGui::PushID(m*1000+c);
                        ImGui::ColorButton("",col,0,ImVec2(10,10));
                        ImGui::PopID();
                        if ((c + 1) % 32 != 0 && c != size - 1)
                            ImGui::SameLine();
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                    ImGui::Unindent();
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::End();
}

bool ShowDatePicker(const char* id, int* level, ImPlotTime* t, const ImPlotTime* t1, const ImPlotTime* t2) {

    ImGui::PushID(id);
    ImGui::BeginGroup();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 col_txt    = style.Colors[ImGuiCol_Text];
    ImVec4 col_dis    = style.Colors[ImGuiCol_TextDisabled];
    ImVec4 col_btn    = style.Colors[ImGuiCol_Button];
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

    const float ht    = ImGui::GetFrameHeight();
    ImVec2 cell_size(ht*1.25f,ht);
    char buff[32];
    bool clk = false;
    tm& Tm = GImPlot->Tm;

    const int min_yr = 1970;
    const int max_yr = 2999;

    // t1 parts
    int t1_mo = 0; int t1_md = 0; int t1_yr = 0;
    if (t1 != nullptr) {
        GetTime(*t1,&Tm);
        t1_mo = Tm.tm_mon;
        t1_md = Tm.tm_mday;
        t1_yr = Tm.tm_year + 1900;
    }

     // t2 parts
    int t2_mo = 0; int t2_md = 0; int t2_yr = 0;
    if (t2 != nullptr) {
        GetTime(*t2,&Tm);
        t2_mo = Tm.tm_mon;
        t2_md = Tm.tm_mday;
        t2_yr = Tm.tm_year + 1900;
    }

    // day widget
    if (*level == 0) {
        *t = FloorTime(*t, ImPlotTimeUnit_Day);
        GetTime(*t, &Tm);
        const int this_year = Tm.tm_year + 1900;
        const int last_year = this_year - 1;
        const int next_year = this_year + 1;
        const int this_mon  = Tm.tm_mon;
        const int last_mon  = this_mon == 0 ? 11 : this_mon - 1;
        const int next_mon  = this_mon == 11 ? 0 : this_mon + 1;
        const int days_this_mo = GetDaysInMonth(this_year, this_mon);
        const int days_last_mo = GetDaysInMonth(this_mon == 0 ? last_year : this_year, last_mon);
        ImPlotTime t_first_mo = FloorTime(*t,ImPlotTimeUnit_Mo);
        GetTime(t_first_mo,&Tm);
        const int first_wd = Tm.tm_wday;
        // month year
        ImFormatString(buff, 32, "%s %d", MONTH_NAMES[this_mon], this_year);
        if (ImGui::Button(buff))
            *level = 1;
        ImGui::SameLine(5*cell_size.x);
        BeginDisabledControls(this_year <= min_yr && this_mon == 0);
        if (ImGui::ArrowButtonEx("##Up",ImGuiDir_Up,cell_size))
            *t = AddTime(*t, ImPlotTimeUnit_Mo, -1);
        EndDisabledControls(this_year <= min_yr && this_mon == 0);
        ImGui::SameLine();
        BeginDisabledControls(this_year >= max_yr && this_mon == 11);
        if (ImGui::ArrowButtonEx("##Down",ImGuiDir_Down,cell_size))
            *t = AddTime(*t, ImPlotTimeUnit_Mo, 1);
        EndDisabledControls(this_year >= max_yr && this_mon == 11);
        // render weekday abbreviations
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        for (int i = 0; i < 7; ++i) {
            ImGui::Button(WD_ABRVS[i],cell_size);
            if (i != 6) { ImGui::SameLine(); }
        }
        ImGui::PopItemFlag();
        // 0 = last mo, 1 = this mo, 2 = next mo
        int mo = first_wd > 0 ? 0 : 1;
        int day = mo == 1 ? 1 : days_last_mo - first_wd + 1;
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 7; ++j) {
                if (mo == 0 && day > days_last_mo) {
                    mo = 1;
                    day = 1;
                }
                else if (mo == 1 && day > days_this_mo) {
                    mo = 2;
                    day = 1;
                }
                const int now_yr = (mo == 0 && this_mon == 0) ? last_year : ((mo == 2 && this_mon == 11) ? next_year : this_year);
                const int now_mo = mo == 0 ? last_mon : (mo == 1 ? this_mon : next_mon);
                const int now_md = day;

                const bool off_mo   = mo == 0 || mo == 2;
                const bool t1_or_t2 = (t1 != nullptr && t1_mo == now_mo && t1_yr == now_yr && t1_md == now_md) ||
                                      (t2 != nullptr && t2_mo == now_mo && t2_yr == now_yr && t2_md == now_md);

                if (off_mo)
                    ImGui::PushStyleColor(ImGuiCol_Text, col_dis);
                if (t1_or_t2) {
                    ImGui::PushStyleColor(ImGuiCol_Button, col_btn);
                    ImGui::PushStyleColor(ImGuiCol_Text, col_txt);
                }
                ImGui::PushID(i*7+j);
                ImFormatString(buff,32,"%d",day);
                if (now_yr == min_yr-1 || now_yr == max_yr+1) {
                    ImGui::Dummy(cell_size);
                }
                else if (ImGui::Button(buff,cell_size) && !clk) {
                    *t = MakeTime(now_yr, now_mo, now_md);
                    clk = true;
                }
                ImGui::PopID();
                if (t1_or_t2)
                    ImGui::PopStyleColor(2);
                if (off_mo)
                    ImGui::PopStyleColor();
                if (j != 6)
                    ImGui::SameLine();
                day++;
            }
        }
    }
    // month widget
    else if (*level == 1) {
        *t = FloorTime(*t, ImPlotTimeUnit_Mo);
        GetTime(*t, &Tm);
        int this_yr  = Tm.tm_year + 1900;
        ImFormatString(buff, 32, "%d", this_yr);
        if (ImGui::Button(buff))
            *level = 2;
        BeginDisabledControls(this_yr <= min_yr);
        ImGui::SameLine(5*cell_size.x);
        if (ImGui::ArrowButtonEx("##Up",ImGuiDir_Up,cell_size))
            *t = AddTime(*t, ImPlotTimeUnit_Yr, -1);
        EndDisabledControls(this_yr <= min_yr);
        ImGui::SameLine();
        BeginDisabledControls(this_yr >= max_yr);
        if (ImGui::ArrowButtonEx("##Down",ImGuiDir_Down,cell_size))
            *t = AddTime(*t, ImPlotTimeUnit_Yr, 1);
        EndDisabledControls(this_yr >= max_yr);
        // ImGui::Dummy(cell_size);
        cell_size.x *= 7.0f/4.0f;
        cell_size.y *= 7.0f/3.0f;
        int mo = 0;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                const bool t1_or_t2 = (t1 != nullptr && t1_yr == this_yr && t1_mo == mo) ||
                                      (t2 != nullptr && t2_yr == this_yr && t2_mo == mo);
                if (t1_or_t2)
                    ImGui::PushStyleColor(ImGuiCol_Button, col_btn);
                if (ImGui::Button(MONTH_ABRVS[mo],cell_size) && !clk) {
                    *t = MakeTime(this_yr, mo);
                    *level = 0;
                }
                if (t1_or_t2)
                    ImGui::PopStyleColor();
                if (j != 3)
                    ImGui::SameLine();
                mo++;
            }
        }
    }
    else if (*level == 2) {
        *t = FloorTime(*t, ImPlotTimeUnit_Yr);
        int this_yr = GetYear(*t);
        int yr = this_yr  - this_yr % 20;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImFormatString(buff,32,"%d-%d",yr,yr+19);
        ImGui::Button(buff);
        ImGui::PopItemFlag();
        ImGui::SameLine(5*cell_size.x);
        BeginDisabledControls(yr <= min_yr);
        if (ImGui::ArrowButtonEx("##Up",ImGuiDir_Up,cell_size))
            *t = MakeTime(yr-20);
        EndDisabledControls(yr <= min_yr);
        ImGui::SameLine();
        BeginDisabledControls(yr + 20 >= max_yr);
        if (ImGui::ArrowButtonEx("##Down",ImGuiDir_Down,cell_size))
            *t = MakeTime(yr+20);
        EndDisabledControls(yr+ 20 >= max_yr);
        // ImGui::Dummy(cell_size);
        cell_size.x *= 7.0f/4.0f;
        cell_size.y *= 7.0f/5.0f;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 4; ++j) {
                const bool t1_or_t2 = (t1 != nullptr && t1_yr == yr) || (t2 != nullptr && t2_yr == yr);
                if (t1_or_t2)
                    ImGui::PushStyleColor(ImGuiCol_Button, col_btn);
                ImFormatString(buff,32,"%d",yr);
                if (yr<1970||yr>3000) {
                    ImGui::Dummy(cell_size);
                }
                else if (ImGui::Button(buff,cell_size)) {
                    *t = MakeTime(yr);
                    *level = 1;
                }
                if (t1_or_t2)
                    ImGui::PopStyleColor();
                if (j != 3)
                    ImGui::SameLine();
                yr++;
            }
        }
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndGroup();
    ImGui::PopID();
    return clk;
}

bool ShowTimePicker(const char* id, ImPlotTime* t) {
    ImPlotContext& gp = *GImPlot;
    ImGui::PushID(id);
    tm& Tm = gp.Tm;
    GetTime(*t,&Tm);

    static const char* nums[] = { "00","01","02","03","04","05","06","07","08","09",
                                  "10","11","12","13","14","15","16","17","18","19",
                                  "20","21","22","23","24","25","26","27","28","29",
                                  "30","31","32","33","34","35","36","37","38","39",
                                  "40","41","42","43","44","45","46","47","48","49",
                                  "50","51","52","53","54","55","56","57","58","59"};

    static const char* am_pm[] = {"am","pm"};

    bool hour24 = gp.Style.Use24HourClock;

    int hr  = hour24 ? Tm.tm_hour : ((Tm.tm_hour == 0 || Tm.tm_hour == 12) ? 12 : Tm.tm_hour % 12);
    int min = Tm.tm_min;
    int sec = Tm.tm_sec;
    int ap  = Tm.tm_hour < 12 ? 0 : 1;

    bool changed = false;

    ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
    spacing.x = 0;
    float width    = ImGui::CalcTextSize("888").x;
    float height   = ImGui::GetFrameHeight();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize,2.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

    ImGui::SetNextItemWidth(width);
    if (ImGui::BeginCombo("##hr",nums[hr],ImGuiComboFlags_NoArrowButton)) {
        const int ia = hour24 ? 0 : 1;
        const int ib = hour24 ? 24 : 13;
        for (int i = ia; i < ib; ++i) {
            if (ImGui::Selectable(nums[i],i==hr)) {
                hr = i;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text(":");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    if (ImGui::BeginCombo("##min",nums[min],ImGuiComboFlags_NoArrowButton)) {
        for (int i = 0; i < 60; ++i) {
            if (ImGui::Selectable(nums[i],i==min)) {
                min = i;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Text(":");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(width);
    if (ImGui::BeginCombo("##sec",nums[sec],ImGuiComboFlags_NoArrowButton)) {
        for (int i = 0; i < 60; ++i) {
            if (ImGui::Selectable(nums[i],i==sec)) {
                sec = i;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    if (!hour24) {
        ImGui::SameLine();
        if (ImGui::Button(am_pm[ap],ImVec2(0,height))) {
            ap = 1 - ap;
            changed = true;
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    ImGui::PopID();

    if (changed) {
        if (!hour24)
            hr = hr % 12 + ap * 12;
        Tm.tm_hour = hr;
        Tm.tm_min  = min;
        Tm.tm_sec  = sec;
        *t = MkTime(&Tm);
    }

    return changed;
}

void StyleColorsAuto(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 0.25f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_FrameBg]       = IMPLOT_AUTO_COL;
    colors[ImPlotCol_PlotBg]        = IMPLOT_AUTO_COL;
    colors[ImPlotCol_PlotBorder]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_LegendBg]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_LegendBorder]  = IMPLOT_AUTO_COL;
    colors[ImPlotCol_LegendText]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_TitleText]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_InlayText]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_PlotBorder]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_AxisText]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_AxisGrid]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_AxisTick]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_AxisBg]        = IMPLOT_AUTO_COL;
    colors[ImPlotCol_AxisBgHovered] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_AxisBgActive]  = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Selection]     = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Crosshairs]    = IMPLOT_AUTO_COL;
}

void StyleColorsClassic(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 0.5f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_FrameBg]       = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_AxisText]      = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImPlotCol_AxisGrid]      = ImVec4(0.90f, 0.90f, 0.90f, 0.25f);
    colors[ImPlotCol_AxisTick]      = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBg]        = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBgHovered] = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBgActive]  = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_Selection]     = ImVec4(0.97f, 0.97f, 0.39f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.50f, 0.50f, 0.50f, 0.75f);
}

void StyleColorsDark(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 0.25f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImPlotCol_LegendText]    = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_AxisText]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_AxisGrid]      = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    colors[ImPlotCol_AxisTick]      = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBg]        = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBgHovered] = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBgActive]  = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
}

void StyleColorsLight(ImPlotStyle* dst) {
    ImPlotStyle* style              = dst ? dst : &ImPlot::GetStyle();
    ImVec4* colors                  = style->Colors;

    style->MinorAlpha               = 1.0f;

    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = IMPLOT_AUTO_COL;
    colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.42f, 0.57f, 1.00f, 0.13f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImPlotCol_LegendBg]      = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.82f, 0.82f, 0.82f, 0.80f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisText]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisGrid]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_AxisTick]      = ImVec4(0.00f, 0.00f, 0.00f, 0.25f);
    colors[ImPlotCol_AxisBg]        = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBgHovered] = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_AxisBgActive]  = IMPLOT_AUTO_COL; // TODO
    colors[ImPlotCol_Selection]     = ImVec4(0.82f, 0.64f, 0.03f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
}

//-----------------------------------------------------------------------------
// [SECTION] Obsolete Functions/Types
//-----------------------------------------------------------------------------

#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS

bool BeginPlot(const char* title, const char* x_label, const char* y1_label, const ImVec2& size,
               ImPlotFlags flags, ImPlotAxisFlags x_flags, ImPlotAxisFlags y1_flags, ImPlotAxisFlags y2_flags, ImPlotAxisFlags y3_flags,
               const char* y2_label, const char* y3_label)
{
    if (!BeginPlot(title, size, flags))
        return false;
    SetupAxis(ImAxis_X1, x_label, x_flags);
    SetupAxis(ImAxis_Y1, y1_label, y1_flags);
    if (ImHasFlag(flags, ImPlotFlags_YAxis2))
        SetupAxis(ImAxis_Y2, y2_label, y2_flags);
    if (ImHasFlag(flags, ImPlotFlags_YAxis3))
        SetupAxis(ImAxis_Y3, y3_label, y3_flags);
    return true;
}

#endif

}  // namespace ImPlot

// MIT License

// Copyright (c) 2023 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.17

// We define this so that the demo does not accidentally use deprecated API
#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#endif

//#include "implot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _MSC_VER
#define sprintf sprintf_s
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define CHECKBOX_FLAG(flags, flag) ImGui::CheckboxFlags(#flag, (unsigned int*)&flags, flag)

// Encapsulates examples for customizing ImPlot.
namespace MyImPlot {

// Example for Custom Data and Getters section.
struct Vector2f {
    Vector2f(float _x, float _y) { x = _x; y = _y; }
    float x, y;
};

// Example for Custom Data and Getters section.
struct WaveData {
    double X, Amp, Freq, Offset;
    WaveData(double x, double amp, double freq, double offset) { X = x; Amp = amp; Freq = freq; Offset = offset; }
};
ImPlotPoint SineWave(int idx, void* wave_data);
ImPlotPoint SawWave(int idx, void* wave_data);
ImPlotPoint Spiral(int idx, void* wave_data);

// Example for Tables section.
void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size);

// Example for Custom Plotters and Tooltips section.
void PlotCandlestick(const char* label_id, const double* xs, const double* opens, const double* closes, const double* lows, const double* highs, int count, bool tooltip = true, float width_percent = 0.25f, ImVec4 bullCol = ImVec4(0,1,0,1), ImVec4 bearCol = ImVec4(1,0,0,1));

// Example for Custom Styles section.
void StyleSeaborn();

} // namespace MyImPlot

namespace ImPlot {

template <typename T>
inline T RandomRange(T min, T max) {
    T scale = rand() / (T) RAND_MAX;
    return min + scale * ( max - min );
}

ImVec4 RandomColor() {
    ImVec4 col;
    col.x = RandomRange(0.0f,1.0f);
    col.y = RandomRange(0.0f,1.0f);
    col.z = RandomRange(0.0f,1.0f);
    col.w = 1.0f;
    return col;
}

double RandomGauss() {
    static double V1, V2, S;
    static int phase = 0;
    double X;
    if(phase == 0) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;
            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
            } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);
    phase = 1 - phase;
    return X;
}

template <int N>
struct NormalDistribution {
    NormalDistribution(double mean, double sd) {
        for (int i = 0; i < N; ++i)
            Data[i] = RandomGauss()*sd + mean;
    }
    double Data[N];
};

// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

// utility structure for realtime plot
struct RollingBuffer {
    float Span;
    ImVector<ImVec2> Data;
    RollingBuffer() {
        Span = 10.0f;
        Data.reserve(2000);
    }
    void AddPoint(float x, float y) {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }
};

// Huge data used by Time Formatting example (~500 MB allocation!)
struct HugeTimeData {
    HugeTimeData(double min) {
        Ts = new double[Size];
        Ys = new double[Size];
        for (int i = 0; i < Size; ++i) {
            Ts[i] = min + i;
            Ys[i] = GetY(Ts[i]);
        }
    }
    ~HugeTimeData() { delete[] Ts;  delete[] Ys; }
    static double GetY(double t) {
        return 0.5 + 0.25 * sin(t/86400/12) +  0.005 * sin(t/3600);
    }
    double* Ts;
    double* Ys;
    static const int Size = 60*60*24*366;
};

//-----------------------------------------------------------------------------
// [SECTION] Demo Functions
//-----------------------------------------------------------------------------

void Demo_Help() {
    ImGui::Text("ABOUT THIS DEMO:");
    ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
    ImGui::BulletText("The \"Tools\" menu above gives access to: Style Editors (ImPlot/ImGui)\n"
                        "and Metrics (general purpose Dear ImGui debugging tool).");
    ImGui::Separator();
    ImGui::Text("PROGRAMMER GUIDE:");
    ImGui::BulletText("See the ShowDemoWindow() code in implot_demo.cpp. <- you are here!");
    ImGui::BulletText("If you see visual artifacts, do one of the following:");
    ImGui::Indent();
    ImGui::BulletText("Handle ImGuiBackendFlags_RendererHasVtxOffset for 16-bit indices in your backend.");
    ImGui::BulletText("Or, enable 32-bit indices in imconfig.h.");
    ImGui::BulletText("Your current configuration is:");
    ImGui::Indent();
    ImGui::BulletText("ImDrawIdx: %d-bit", (int)(sizeof(ImDrawIdx) * 8));
    ImGui::BulletText("ImGuiBackendFlags_RendererHasVtxOffset: %s", (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) ? "True" : "False");
    ImGui::Unindent();
    ImGui::Unindent();
    ImGui::Separator();
    ImGui::Text("USER GUIDE:");
    ShowUserGuide();
}

//-----------------------------------------------------------------------------

void ButtonSelector(const char* label, ImGuiMouseButton* b) {
    ImGui::PushID(label);
    if (ImGui::RadioButton("LMB",*b == ImGuiMouseButton_Left))
        *b = ImGuiMouseButton_Left;
    ImGui::SameLine();
    if (ImGui::RadioButton("RMB",*b == ImGuiMouseButton_Right))
        *b = ImGuiMouseButton_Right;
    ImGui::SameLine();
    if (ImGui::RadioButton("MMB",*b == ImGuiMouseButton_Middle))
        *b = ImGuiMouseButton_Middle;
    ImGui::PopID();
}

void ModSelector(const char* label, int* k) {
    ImGui::PushID(label);
    ImGui::CheckboxFlags("Ctrl", (unsigned int*)k, ImGuiMod_Ctrl); ImGui::SameLine();
    ImGui::CheckboxFlags("Shift", (unsigned int*)k, ImGuiMod_Shift); ImGui::SameLine();
    ImGui::CheckboxFlags("Alt", (unsigned int*)k, ImGuiMod_Alt); ImGui::SameLine();
    ImGui::CheckboxFlags("Super", (unsigned int*)k, ImGuiMod_Super);
    ImGui::PopID();
}

void InputMapping(const char* label, ImGuiMouseButton* b, int* k) {
    ImGui::LabelText("##","%s",label);
    if (b != nullptr) {
        ImGui::SameLine(100);
        ButtonSelector(label,b);
    }
    if (k != nullptr) {
        ImGui::SameLine(300);
        ModSelector(label,k);
    }
}

void ShowInputMapping() {
    ImPlotInputMap& map = ImPlot::GetInputMap();
    InputMapping("Pan",&map.Pan,&map.PanMod);
    InputMapping("Fit",&map.Fit,nullptr);
    InputMapping("Select",&map.Select,&map.SelectMod);
    InputMapping("SelectHorzMod",nullptr,&map.SelectHorzMod);
    InputMapping("SelectVertMod",nullptr,&map.SelectVertMod);
    InputMapping("SelectCancel",&map.SelectCancel,nullptr);
    InputMapping("Menu",&map.Menu,nullptr);
    InputMapping("OverrideMod",nullptr,&map.OverrideMod);
    InputMapping("ZoomMod",nullptr,&map.ZoomMod);
    ImGui::SliderFloat("ZoomRate",&map.ZoomRate,-1,1);
}

void Demo_Config() {
    ImGui::ShowFontSelector("Font");
    ImGui::ShowStyleSelector("ImGui Style");
    ImPlot::ShowStyleSelector("ImPlot Style");
    ImPlot::ShowColormapSelector("ImPlot Colormap");
    ImPlot::ShowInputMapSelector("Input Map");
    ImGui::Separator();
    ImGui::Checkbox("Use Local Time", &ImPlot::GetStyle().UseLocalTime);
    ImGui::Checkbox("Use ISO 8601", &ImPlot::GetStyle().UseISO8601);
    ImGui::Checkbox("Use 24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);
    ImGui::Separator();
    if (ImPlot::BeginPlot("Preview")) {
        static double now = (double)time(nullptr);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisLimits(ImAxis_X1, now, now + 24*3600);
        for (int i = 0; i < 10; ++i) {
            double x[2] = {now, now + 24*3600};
            double y[2] = {0,i/9.0};
            ImGui::PushID(i);
            ImPlot::PlotLine("##Line",x,y,2);
            ImGui::PopID();
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_LinePlots() {
    static float xs1[1001], ys1[1001];
    for (int i = 0; i < 1001; ++i) {
        xs1[i] = i * 0.001f;
        ys1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
    }
    static double xs2[20], ys2[20];
    for (int i = 0; i < 20; ++i) {
        xs2[i] = i * 1/19.0f;
        ys2[i] = xs2[i] * xs2[i];
    }
    if (ImPlot::BeginPlot("Line Plots")) {
        ImPlot::SetupAxes("x","y");
        ImPlot::PlotLine("f(x)", xs1, ys1, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_FilledLinePlots() {
    static double xs1[101], ys1[101], ys2[101], ys3[101];
    srand(0);
    for (int i = 0; i < 101; ++i) {
        xs1[i] = (float)i;
        ys1[i] = RandomRange(400.0,450.0);
        ys2[i] = RandomRange(275.0,350.0);
        ys3[i] = RandomRange(150.0,225.0);
    }
    static bool show_lines = true;
    static bool show_fills = true;
    static float fill_ref = 0;
    static int shade_mode = 0;
    static ImPlotShadedFlags flags = 0;
    ImGui::Checkbox("Lines",&show_lines); ImGui::SameLine();
    ImGui::Checkbox("Fills",&show_fills);
    if (show_fills) {
        ImGui::SameLine();
        if (ImGui::RadioButton("To -INF",shade_mode == 0))
            shade_mode = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton("To +INF",shade_mode == 1))
            shade_mode = 1;
        ImGui::SameLine();
        if (ImGui::RadioButton("To Ref",shade_mode == 2))
            shade_mode = 2;
        if (shade_mode == 2) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::DragFloat("##Ref",&fill_ref, 1, -100, 500);
        }
    }

    if (ImPlot::BeginPlot("Stock Prices")) {
        ImPlot::SetupAxes("Days","Price");
        ImPlot::SetupAxesLimits(0,100,0,500);
        if (show_fills) {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("Stock 1", xs1, ys1, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref, flags);
            ImPlot::PlotShaded("Stock 2", xs1, ys2, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref, flags);
            ImPlot::PlotShaded("Stock 3", xs1, ys3, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref, flags);
            ImPlot::PopStyleVar();
        }
        if (show_lines) {
            ImPlot::PlotLine("Stock 1", xs1, ys1, 101);
            ImPlot::PlotLine("Stock 2", xs1, ys2, 101);
            ImPlot::PlotLine("Stock 3", xs1, ys3, 101);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_ShadedPlots() {
    static float xs[1001], ys[1001], ys1[1001], ys2[1001], ys3[1001], ys4[1001];
    srand(0);
    for (int i = 0; i < 1001; ++i) {
        xs[i]  = i * 0.001f;
        ys[i]  = 0.25f + 0.25f * sinf(25 * xs[i]) * sinf(5 * xs[i]) + RandomRange(-0.01f, 0.01f);
        ys1[i] = ys[i] + RandomRange(0.1f, 0.12f);
        ys2[i] = ys[i] - RandomRange(0.1f, 0.12f);
        ys3[i] = 0.75f + 0.2f * sinf(25 * xs[i]);
        ys4[i] = 0.75f + 0.1f * cosf(25 * xs[i]);
    }
    static float alpha = 0.25f;
    ImGui::DragFloat("Alpha",&alpha,0.01f,0,1);

    if (ImPlot::BeginPlot("Shaded Plots")) {
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, alpha);
        ImPlot::PlotShaded("Uncertain Data",xs,ys1,ys2,1001);
        ImPlot::PlotLine("Uncertain Data", xs, ys, 1001);
        ImPlot::PlotShaded("Overlapping",xs,ys3,ys4,1001);
        ImPlot::PlotLine("Overlapping",xs,ys3,1001);
        ImPlot::PlotLine("Overlapping",xs,ys4,1001);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_ScatterPlots() {
    srand(0);
    static float xs1[100], ys1[100];
    for (int i = 0; i < 100; ++i) {
        xs1[i] = i * 0.01f;
        ys1[i] = xs1[i] + 0.1f * ((float)rand() / (float)RAND_MAX);
    }
    static float xs2[50], ys2[50];
    for (int i = 0; i < 50; i++) {
        xs2[i] = 0.25f + 0.2f * ((float)rand() / (float)RAND_MAX);
        ys2[i] = 0.75f + 0.2f * ((float)rand() / (float)RAND_MAX);
    }

    if (ImPlot::BeginPlot("Scatter Plot")) {
        ImPlot::PlotScatter("Data 1", xs1, ys1, 100);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6, ImPlot::GetColormapColor(1), IMPLOT_AUTO, ImPlot::GetColormapColor(1));
        ImPlot::PlotScatter("Data 2", xs2, ys2, 50);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_StairstepPlots() {
    static float ys1[21], ys2[21];
    for (int i = 0; i < 21; ++i) {
        ys1[i] = 0.75f + 0.2f * sinf(10 * i * 0.05f);
        ys2[i] = 0.25f + 0.2f * sinf(10 * i * 0.05f);
    }
    static ImPlotStairsFlags flags = 0;
    CHECKBOX_FLAG(flags, ImPlotStairsFlags_Shaded);
    if (ImPlot::BeginPlot("Stairstep Plot")) {
        ImPlot::SetupAxes("x","f(x)");
        ImPlot::SetupAxesLimits(0,1,0,1);

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5f,0.5f,0.5f,1.0f));
        ImPlot::PlotLine("##1",ys1,21,0.05f);
        ImPlot::PlotLine("##2",ys2,21,0.05f);
        ImPlot::PopStyleColor();

        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
        ImPlot::PlotStairs("Post Step (default)", ys1, 21, 0.05f, 0, flags);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
        ImPlot::PlotStairs("Pre Step", ys2, 21, 0.05f, 0, flags|ImPlotStairsFlags_PreStep);

        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_BarPlots() {
    static ImS8  data[10] = {1,2,3,4,5,6,7,8,9,10};
    if (ImPlot::BeginPlot("Bar Plot")) {
        ImPlot::PlotBars("Vertical",data,10,0.7,1);
        ImPlot::PlotBars("Horizontal",data,10,0.4,1,ImPlotBarsFlags_Horizontal);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_BarGroups() {
    static ImS8  data[30] = {83, 67, 23, 89, 83, 78, 91, 82, 85, 90,  // midterm
                             80, 62, 56, 99, 55, 78, 88, 78, 90, 100, // final
                             80, 69, 52, 92, 72, 78, 75, 76, 89, 95}; // course

    static const char*  ilabels[]   = {"Midterm Exam","Final Exam","Course Grade"};
    static const char*  glabels[]   = {"S1","S2","S3","S4","S5","S6","S7","S8","S9","S10"};
    static const double positions[] = {0,1,2,3,4,5,6,7,8,9};

    static int items  = 3;
    static int groups = 10;
    static float size = 0.67f;

    static ImPlotBarGroupsFlags flags = 0;
    static bool horz = false;

    ImGui::CheckboxFlags("Stacked", (unsigned int*)&flags, ImPlotBarGroupsFlags_Stacked);
    ImGui::SameLine();
    ImGui::Checkbox("Horizontal",&horz);

    ImGui::SliderInt("Items",&items,1,3);
    ImGui::SliderFloat("Size",&size,0,1);

    if (ImPlot::BeginPlot("Bar Group")) {
        ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        if (horz) {
            ImPlot::SetupAxes("Score","Student",ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisTicks(ImAxis_Y1,positions, groups, glabels);
            ImPlot::PlotBarGroups(ilabels,data,items,groups,size,0,flags|ImPlotBarGroupsFlags_Horizontal);
        }
        else {
            ImPlot::SetupAxes("Student","Score",ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisTicks(ImAxis_X1,positions, groups, glabels);
            ImPlot::PlotBarGroups(ilabels,data,items,groups,size,0,flags);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_BarStacks() {

    static ImPlotColormap Liars = -1;
    if (Liars == -1) {
        static const ImU32 Liars_Data[6] = { 4282515870, 4282609140, 4287357182, 4294630301, 4294945280, 4294921472 };
        Liars = ImPlot::AddColormap("Liars", Liars_Data, 6);
    }

    static bool diverging = true;
    ImGui::Checkbox("Diverging",&diverging);

    static const char* politicians[] = {"Trump","Bachman","Cruz","Gingrich","Palin","Santorum","Walker","Perry","Ryan","McCain","Rubio","Romney","Rand Paul","Christie","Biden","Kasich","Sanders","J Bush","H Clinton","Obama"};
    static int data_reg[] = {18,26,7,14,10,8,6,11,4,4,3,8,6,8,6,5,0,3,1,2,                // Pants on Fire
                             43,36,30,21,30,27,25,17,11,22,15,16,16,17,12,12,14,6,13,12,  // False
                             16,13,28,22,15,21,15,18,30,17,24,18,13,10,14,15,17,22,14,12, // Mostly False
                             17,10,13,25,12,22,19,26,23,17,22,27,20,26,29,17,18,22,21,27, // Half True
                             5,7,16,10,10,12,23,13,17,20,22,16,23,19,20,26,36,29,27,26,   // Mostly True
                             1,8,6,8,23,10,12,15,15,20,14,15,22,20,19,25,15,18,24,21};    // True
    static const char* labels_reg[] = {"Pants on Fire","False","Mostly False","Half True","Mostly True","True"};


    static int data_div[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                         // Pants on Fire (dummy, to order legend logically)
                             0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                         // False         (dummy, to order legend logically)
                             0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                         // Mostly False  (dummy, to order legend logically)
                             -16,-13,-28,-22,-15,-21,-15,-18,-30,-17,-24,-18,-13,-10,-14,-15,-17,-22,-14,-12, // Mostly False
                             -43,-36,-30,-21,-30,-27,-25,-17,-11,-22,-15,-16,-16,-17,-12,-12,-14,-6,-13,-12,  // False
                             -18,-26,-7,-14,-10,-8,-6,-11,-4,-4,-3,-8,-6,-8,-6,-5,0,-3,-1,-2,                 // Pants on Fire
                             17,10,13,25,12,22,19,26,23,17,22,27,20,26,29,17,18,22,21,27,                     // Half True
                             5,7,16,10,10,12,23,13,17,20,22,16,23,19,20,26,36,29,27,26,                       // Mostly True
                             1,8,6,8,23,10,12,15,15,20,14,15,22,20,19,25,15,18,24,21};                        // True
    static const char* labels_div[] = {"Pants on Fire","False","Mostly False","Mostly False","False","Pants on Fire","Half True","Mostly True","True"};

    ImPlot::PushColormap(Liars);
    if (ImPlot::BeginPlot("PolitiFact: Who Lies More?",ImVec2(-1,400),ImPlotFlags_NoMouseText)) {
        ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside|ImPlotLegendFlags_Horizontal);
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_Invert);
        ImPlot::SetupAxisTicks(ImAxis_Y1,0,19,20,politicians,false);
        if (diverging)
            ImPlot::PlotBarGroups(labels_div,data_div,9,20,0.75,0,ImPlotBarGroupsFlags_Stacked|ImPlotBarGroupsFlags_Horizontal);
        else
            ImPlot::PlotBarGroups(labels_reg,data_reg,6,20,0.75,0,ImPlotBarGroupsFlags_Stacked|ImPlotBarGroupsFlags_Horizontal);
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();
}

//-----------------------------------------------------------------------------

void Demo_ErrorBars() {
    static float xs[5]    = {1,2,3,4,5};
    static float bar[5]   = {1,2,5,3,4};
    static float lin1[5]  = {8,8,9,7,8};
    static float lin2[5]  = {6,7,6,9,6};
    static float err1[5]  = {0.2f, 0.4f, 0.2f, 0.6f, 0.4f};
    static float err2[5]  = {0.4f, 0.2f, 0.4f, 0.8f, 0.6f};
    static float err3[5]  = {0.09f, 0.14f, 0.09f, 0.12f, 0.16f};
    static float err4[5]  = {0.02f, 0.08f, 0.15f, 0.05f, 0.2f};


    if (ImPlot::BeginPlot("##ErrorBars")) {
        ImPlot::SetupAxesLimits(0, 6, 0, 10);
        ImPlot::PlotBars("Bar", xs, bar, 5, 0.5f);
        ImPlot::PlotErrorBars("Bar", xs, bar, err1, 5);
        ImPlot::SetNextErrorBarStyle(ImPlot::GetColormapColor(1), 0);
        ImPlot::PlotErrorBars("Line", xs, lin1, err1, err2, 5);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
        ImPlot::PlotLine("Line", xs, lin1, 5);
        ImPlot::PushStyleColor(ImPlotCol_ErrorBar, ImPlot::GetColormapColor(2));
        ImPlot::PlotErrorBars("Scatter", xs, lin2, err2, 5);
        ImPlot::PlotErrorBars("Scatter", xs, lin2,  err3, err4, 5, ImPlotErrorBarsFlags_Horizontal);
        ImPlot::PopStyleColor();
        ImPlot::PlotScatter("Scatter", xs, lin2, 5);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_StemPlots() {
    static double xs[51], ys1[51], ys2[51];
    for (int i = 0; i < 51; ++i) {
        xs[i] = i * 0.02;
        ys1[i] = 1.0 + 0.5 * sin(25*xs[i])*cos(2*xs[i]);
        ys2[i] = 0.5 + 0.25  * sin(10*xs[i]) * sin(xs[i]);
    }
    if (ImPlot::BeginPlot("Stem Plots")) {
        ImPlot::SetupAxisLimits(ImAxis_X1,0,1.0);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1.6);
        ImPlot::PlotStems("Stems 1",xs,ys1,51);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::PlotStems("Stems 2", xs, ys2,51);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_InfiniteLines() {
    static double vals[] = {0.25, 0.5, 0.75};
    if (ImPlot::BeginPlot("##Infinite")) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoInitialFit,ImPlotAxisFlags_NoInitialFit);
        ImPlot::PlotInfLines("Vertical",vals,3);
        ImPlot::PlotInfLines("Horizontal",vals,3,ImPlotInfLinesFlags_Horizontal);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_PieCharts() {
    static const char* labels1[]    = {"Frogs","Hogs","Dogs","Logs"};
    static float data1[]            = {0.15f,  0.30f,  0.2f, 0.05f};
    static ImPlotPieChartFlags flags = 0;
    ImGui::SetNextItemWidth(250);
    ImGui::DragFloat4("Values", data1, 0.01f, 0, 1);
    CHECKBOX_FLAG(flags, ImPlotPieChartFlags_Normalize);
    CHECKBOX_FLAG(flags, ImPlotPieChartFlags_IgnoreHidden);

    if (ImPlot::BeginPlot("##Pie1", ImVec2(250,250), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotPieChart(labels1, data1, 4, 0.5, 0.5, 0.4, "%.2f", 90, flags);
        ImPlot::EndPlot();
    }

    ImGui::SameLine();

    static const char* labels2[]   = {"A","B","C","D","E"};
    static int data2[]             = {1,1,2,3,5};

    ImPlot::PushColormap(ImPlotColormap_Pastel);
    if (ImPlot::BeginPlot("##Pie2", ImVec2(250,250), ImPlotFlags_Equal | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 1, 0, 1);
        ImPlot::PlotPieChart(labels2, data2, 5, 0.5, 0.5, 0.4, "%.0f", 180, flags);
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();
}

//-----------------------------------------------------------------------------

void Demo_Heatmaps() {
    static float values1[7][7]  = {{0.8f, 2.4f, 2.5f, 3.9f, 0.0f, 4.0f, 0.0f},
                                    {2.4f, 0.0f, 4.0f, 1.0f, 2.7f, 0.0f, 0.0f},
                                    {1.1f, 2.4f, 0.8f, 4.3f, 1.9f, 4.4f, 0.0f},
                                    {0.6f, 0.0f, 0.3f, 0.0f, 3.1f, 0.0f, 0.0f},
                                    {0.7f, 1.7f, 0.6f, 2.6f, 2.2f, 6.2f, 0.0f},
                                    {1.3f, 1.2f, 0.0f, 0.0f, 0.0f, 3.2f, 5.1f},
                                    {0.1f, 2.0f, 0.0f, 1.4f, 0.0f, 1.9f, 6.3f}};
    static float scale_min       = 0;
    static float scale_max       = 6.3f;
    static const char* xlabels[] = {"C1","C2","C3","C4","C5","C6","C7"};
    static const char* ylabels[] = {"R1","R2","R3","R4","R5","R6","R7"};

    static ImPlotColormap map = ImPlotColormap_Viridis;
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map),ImVec2(225,0),map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        // We bust the color cache of our plots so that item colors will
        // resample the new colormap in the event that they have already
        // been created. See documentation in implot.h.
        BustColorCache("##Heatmap1");
        BustColorCache("##Heatmap2");
    }

    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImGui::SetNextItemWidth(225);
    ImGui::DragFloatRange2("Min / Max",&scale_min, &scale_max, 0.01f, -20, 20);

    static ImPlotHeatmapFlags hm_flags = 0;

    ImGui::CheckboxFlags("Column Major", (unsigned int*)&hm_flags, ImPlotHeatmapFlags_ColMajor);

    static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImPlot::PushColormap(map);

    if (ImPlot::BeginPlot("##Heatmap1",ImVec2(225,225),ImPlotFlags_NoLegend|ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxes(nullptr, nullptr, axes_flags, axes_flags);
        ImPlot::SetupAxisTicks(ImAxis_X1,0 + 1.0/14.0, 1 - 1.0/14.0, 7, xlabels);
        ImPlot::SetupAxisTicks(ImAxis_Y1,1 - 1.0/14.0, 0 + 1.0/14.0, 7, ylabels);
        ImPlot::PlotHeatmap("heat",values1[0],7,7,scale_min,scale_max,"%g",ImPlotPoint(0,0),ImPlotPoint(1,1),hm_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale",scale_min, scale_max, ImVec2(60,225));

    ImGui::SameLine();

    const int size = 80;
    static double values2[size*size];
    srand((unsigned int)(ImGui::GetTime()*1000000));
    for (int i = 0; i < size*size; ++i)
        values2[i] = RandomRange(0.0,1.0);

    if (ImPlot::BeginPlot("##Heatmap2",ImVec2(225,225))) {
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(-1,1,-1,1);
        ImPlot::PlotHeatmap("heat1",values2,size,size,0,1,nullptr);
        ImPlot::PlotHeatmap("heat2",values2,size,size,0,1,nullptr, ImPlotPoint(-1,-1), ImPlotPoint(0,0));
        ImPlot::EndPlot();
    }
    ImPlot::PopColormap();

}

//-----------------------------------------------------------------------------

void Demo_Histogram() {
    static ImPlotHistogramFlags hist_flags = ImPlotHistogramFlags_Density;
    static int  bins       = 50;
    static double mu       = 5;
    static double sigma    = 2;
    ImGui::SetNextItemWidth(200);
    if (ImGui::RadioButton("Sqrt",bins==ImPlotBin_Sqrt))       { bins = ImPlotBin_Sqrt;    } ImGui::SameLine();
    if (ImGui::RadioButton("Sturges",bins==ImPlotBin_Sturges)) { bins = ImPlotBin_Sturges; } ImGui::SameLine();
    if (ImGui::RadioButton("Rice",bins==ImPlotBin_Rice))       { bins = ImPlotBin_Rice;    } ImGui::SameLine();
    if (ImGui::RadioButton("Scott",bins==ImPlotBin_Scott))     { bins = ImPlotBin_Scott;   } ImGui::SameLine();
    if (ImGui::RadioButton("N Bins",bins>=0))                  { bins = 50;                }
    if (bins>=0) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::SliderInt("##Bins", &bins, 1, 100);
    }
    ImGui::CheckboxFlags("Horizontal", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Horizontal);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Density", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Density);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Cumulative", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Cumulative);

    static bool range = false;
    ImGui::Checkbox("Range", &range);
    static float rmin = -3;
    static float rmax = 13;
    if (range) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::DragFloat2("##Range",&rmin,0.1f,-3,13);
        ImGui::SameLine();
        ImGui::CheckboxFlags("Exclude Outliers", (unsigned int*)&hist_flags, ImPlotHistogramFlags_NoOutliers);
    }
    static NormalDistribution<10000> dist(mu, sigma);
    static double x[100];
    static double y[100];
    if (hist_flags & ImPlotHistogramFlags_Density) {
        for (int i = 0; i < 100; ++i) {
            x[i] = -3 + 16 * (double)i/99.0;
            y[i] = exp( - (x[i]-mu)*(x[i]-mu) / (2*sigma*sigma)) / (sigma * sqrt(2*3.141592653589793238));
        }
        if (hist_flags & ImPlotHistogramFlags_Cumulative) {
            for (int i = 1; i < 100; ++i)
                y[i] += y[i-1];
            for (int i = 0; i < 100; ++i)
                y[i] /= y[99];
        }
    }

    if (ImPlot::BeginPlot("##Histograms")) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
        ImPlot::PlotHistogram("Empirical", dist.Data, 10000, bins, 1.0, range ? ImPlotRange(rmin,rmax) : ImPlotRange(), hist_flags);
        if ((hist_flags & ImPlotHistogramFlags_Density) && !(hist_flags & ImPlotHistogramFlags_NoOutliers)) {
            if (hist_flags & ImPlotHistogramFlags_Horizontal)
                ImPlot::PlotLine("Theoretical",y,x,100);
            else
                ImPlot::PlotLine("Theoretical",x,y,100);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_Histogram2D() {
    static int count     = 50000;
    static int xybins[2] = {100,100};

    static ImPlotHistogramFlags hist_flags = 0;

    ImGui::SliderInt("Count",&count,100,100000);
    ImGui::SliderInt2("Bins",xybins,1,500);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Density", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Density);

    static NormalDistribution<100000> dist1(1, 2);
    static NormalDistribution<100000> dist2(1, 1);
    double max_count = 0;
    ImPlotAxisFlags flags = ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_Foreground;
    ImPlot::PushColormap("Hot");
    if (ImPlot::BeginPlot("##Hist2D",ImVec2(ImGui::GetContentRegionAvail().x-100-ImGui::GetStyle().ItemSpacing.x,0))) {
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxesLimits(-6,6,-6,6);
        max_count = ImPlot::PlotHistogram2D("Hist2D",dist1.Data,dist2.Data,count,xybins[0],xybins[1],ImPlotRect(-6,6,-6,6), hist_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale(hist_flags & ImPlotHistogramFlags_Density ? "Density" : "Count",0,max_count,ImVec2(100,0));
    ImPlot::PopColormap();
}

//-----------------------------------------------------------------------------

void Demo_DigitalPlots() {
    ImGui::BulletText("Digital plots do not respond to Y drag and zoom, so that");
    ImGui::Indent();
    ImGui::Text("you can drag analog plots over the rising/falling digital edge.");
    ImGui::Unindent();

    static bool paused = false;
    static ScrollingBuffer dataDigital[2];
    static ScrollingBuffer dataAnalog[2];
    static bool showDigital[2] = {true, false};
    static bool showAnalog[2] = {true, false};

    char label[32];
    ImGui::Checkbox("digital_0", &showDigital[0]); ImGui::SameLine();
    ImGui::Checkbox("digital_1", &showDigital[1]); ImGui::SameLine();
    ImGui::Checkbox("analog_0",  &showAnalog[0]);  ImGui::SameLine();
    ImGui::Checkbox("analog_1",  &showAnalog[1]);

    static float t = 0;
    if (!paused) {
        t += ImGui::GetIO().DeltaTime;
        //digital signal values
        if (showDigital[0])
            dataDigital[0].AddPoint(t, sinf(2*t) > 0.45);
        if (showDigital[1])
            dataDigital[1].AddPoint(t, sinf(2*t) < 0.45);
        //Analog signal values
        if (showAnalog[0])
            dataAnalog[0].AddPoint(t, sinf(2*t));
        if (showAnalog[1])
            dataAnalog[1].AddPoint(t, cosf(2*t));
    }
    if (ImPlot::BeginPlot("##Digital")) {
        ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0, t, paused ? ImGuiCond_Once : ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);
        for (int i = 0; i < 2; ++i) {
            if (showDigital[i] && dataDigital[i].Data.size() > 0) {
                snprintf(label, sizeof(label), "digital_%d", i);
                ImPlot::PlotDigital(label, &dataDigital[i].Data[0].x, &dataDigital[i].Data[0].y, dataDigital[i].Data.size(), 0, dataDigital[i].Offset, 2 * sizeof(float));
            }
        }
        for (int i = 0; i < 2; ++i) {
            if (showAnalog[i]) {
                snprintf(label, sizeof(label), "analog_%d", i);
                if (dataAnalog[i].Data.size() > 0)
                    ImPlot::PlotLine(label, &dataAnalog[i].Data[0].x, &dataAnalog[i].Data[0].y, dataAnalog[i].Data.size(), 0, dataAnalog[i].Offset, 2 * sizeof(float));
            }
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_Images() {
    ImGui::BulletText("Below we are displaying the font texture, which is the only texture we have\naccess to in this demo.");
    ImGui::BulletText("Use the 'ImTextureID' type as storage to pass pointers or identifiers to your\nown texture data.");
    ImGui::BulletText("See ImGui Wiki page 'Image Loading and Displaying Examples'.");
    static ImVec2 bmin(0,0);
    static ImVec2 bmax(1,1);
    static ImVec2 uv0(0,0);
    static ImVec2 uv1(1,1);
    static ImVec4 tint(1,1,1,1);
    ImGui::SliderFloat2("Min", &bmin.x, -2, 2, "%.1f");
    ImGui::SliderFloat2("Max", &bmax.x, -2, 2, "%.1f");
    ImGui::SliderFloat2("UV0", &uv0.x, -2, 2, "%.1f");
    ImGui::SliderFloat2("UV1", &uv1.x, -2, 2, "%.1f");
    ImGui::ColorEdit4("Tint",&tint.x);
    if (ImPlot::BeginPlot("##image")) {
        ImPlot::PlotImage("my image",ImGui::GetIO().Fonts->TexID, bmin, bmax, uv0, uv1, tint);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_RealtimePlots() {
    ImGui::BulletText("Move your mouse to change the data!");
    ImGui::BulletText("This example assumes 60 FPS. Higher FPS requires larger buffer size.");
    static ScrollingBuffer sdata1, sdata2;
    static RollingBuffer   rdata1, rdata2;
    ImVec2 mouse = ImGui::GetMousePos();
    static float t = 0;
    t += ImGui::GetIO().DeltaTime;
    sdata1.AddPoint(t, mouse.x * 0.0005f);
    rdata1.AddPoint(t, mouse.x * 0.0005f);
    sdata2.AddPoint(t, mouse.y * 0.0005f);
    rdata2.AddPoint(t, mouse.y * 0.0005f);

    static float history = 10.0f;
    ImGui::SliderFloat("History",&history,1,30,"%.1f s");
    rdata1.Span = history;
    rdata2.Span = history;

    static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

    if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1,150))) {
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1,t - history, t, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
        ImPlot::PlotShaded("Mouse X", &sdata1.Data[0].x, &sdata1.Data[0].y, sdata1.Data.size(), -INFINITY, 0, sdata1.Offset, 2 * sizeof(float));
        ImPlot::PlotLine("Mouse Y", &sdata2.Data[0].x, &sdata2.Data[0].y, sdata2.Data.size(), 0, sdata2.Offset, 2*sizeof(float));
        ImPlot::EndPlot();
    }
    if (ImPlot::BeginPlot("##Rolling", ImVec2(-1,150))) {
        ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1,0,history, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1,0,1);
        ImPlot::PlotLine("Mouse X", &rdata1.Data[0].x, &rdata1.Data[0].y, rdata1.Data.size(), 0, 0, 2 * sizeof(float));
        ImPlot::PlotLine("Mouse Y", &rdata2.Data[0].x, &rdata2.Data[0].y, rdata2.Data.size(), 0, 0, 2 * sizeof(float));
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_MarkersAndText() {
    static float mk_size = ImPlot::GetStyle().MarkerSize;
    static float mk_weight = ImPlot::GetStyle().MarkerWeight;
    ImGui::DragFloat("Marker Size",&mk_size,0.1f,2.0f,10.0f,"%.2f px");
    ImGui::DragFloat("Marker Weight", &mk_weight,0.05f,0.5f,3.0f,"%.2f px");

    if (ImPlot::BeginPlot("##MarkerStyles", ImVec2(-1,0), ImPlotFlags_CanvasOnly)) {

        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, 10, 0, 12);

        ImS8 xs[2] = {1,4};
        ImS8 ys[2] = {10,11};

        // filled markers
        for (int m = 0; m < ImPlotMarker_COUNT; ++m) {
            ImGui::PushID(m);
            ImPlot::SetNextMarkerStyle(m, mk_size, IMPLOT_AUTO_COL, mk_weight);
            ImPlot::PlotLine("##Filled", xs, ys, 2);
            ImGui::PopID();
            ys[0]--; ys[1]--;
        }
        xs[0] = 6; xs[1] = 9; ys[0] = 10; ys[1] = 11;
        // open markers
        for (int m = 0; m < ImPlotMarker_COUNT; ++m) {
            ImGui::PushID(m);
            ImPlot::SetNextMarkerStyle(m, mk_size, ImVec4(0,0,0,0), mk_weight);
            ImPlot::PlotLine("##Open", xs, ys, 2);
            ImGui::PopID();
            ys[0]--; ys[1]--;
        }

        ImPlot::PlotText("Filled Markers", 2.5f, 6.0f);
        ImPlot::PlotText("Open Markers",   7.5f, 6.0f);

        ImPlot::PushStyleColor(ImPlotCol_InlayText, ImVec4(1,0,1,1));
        ImPlot::PlotText("Vertical Text", 5.0f, 6.0f, ImVec2(0,0), ImPlotTextFlags_Vertical);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_NaNValues() {

    static bool include_nan = true;
    static ImPlotLineFlags flags = 0;

    float data1[5] = {0.0f,0.25f,0.5f,0.75f,1.0f};
    float data2[5] = {0.0f,0.25f,0.5f,0.75f,1.0f};

    if (include_nan)
        data1[2] = NAN;

    ImGui::Checkbox("Include NaN",&include_nan);
    ImGui::SameLine();
    ImGui::CheckboxFlags("Skip NaN", (unsigned int*)&flags, ImPlotLineFlags_SkipNaN);

    if (ImPlot::BeginPlot("##NaNValues")) {
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
        ImPlot::PlotLine("line", data1, data2, 5, flags);
        ImPlot::PlotBars("bars", data1, 5);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_LogScale() {
    static double xs[1001], ys1[1001], ys2[1001], ys3[1001];
    for (int i = 0; i < 1001; ++i) {
        xs[i]  = i*0.1f;
        ys1[i] = sin(xs[i]) + 1;
        ys2[i] = log(xs[i]);
        ys3[i] = pow(10.0, xs[i]);
    }
    if (ImPlot::BeginPlot("Log Plot", ImVec2(-1,0))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxesLimits(0.1, 100, 0, 10);
        ImPlot::PlotLine("f(x) = x",        xs, xs,  1001);
        ImPlot::PlotLine("f(x) = sin(x)+1", xs, ys1, 1001);
        ImPlot::PlotLine("f(x) = log(x)",   xs, ys2, 1001);
        ImPlot::PlotLine("f(x) = 10^x",     xs, ys3, 21);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_SymmetricLogScale() {
    static double xs[1001], ys1[1001], ys2[1001];
    for (int i = 0; i < 1001; ++i) {
        xs[i]  = i*0.1f-50;
        ys1[i] = sin(xs[i]);
        ys2[i] = i*0.002 - 1;
    }
    if (ImPlot::BeginPlot("SymLog Plot", ImVec2(-1,0))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_SymLog);
        ImPlot::PlotLine("f(x) = a*x+b",xs,ys2,1001);
        ImPlot::PlotLine("f(x) = sin(x)",xs,ys1,1001);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_TimeScale() {

    static double t_min = 1609459200; // 01/01/2021 @ 12:00:00am (UTC)
    static double t_max = 1640995200; // 01/01/2022 @ 12:00:00am (UTC)

    ImGui::BulletText("When ImPlotAxisFlags_Time is enabled on the X-Axis, values are interpreted as\n"
                        "UNIX timestamps in seconds and axis labels are formated as date/time.");
    ImGui::BulletText("By default, labels are in UTC time but can be set to use local time instead.");

    ImGui::Checkbox("Local Time",&ImPlot::GetStyle().UseLocalTime);
    ImGui::SameLine();
    ImGui::Checkbox("ISO 8601",&ImPlot::GetStyle().UseISO8601);
    ImGui::SameLine();
    ImGui::Checkbox("24 Hour Clock",&ImPlot::GetStyle().Use24HourClock);

    static HugeTimeData* data = nullptr;
    if (data == nullptr) {
        ImGui::SameLine();
        if (ImGui::Button("Generate Huge Data (~500MB!)")) {
            static HugeTimeData sdata(t_min);
            data = &sdata;
        }
    }

    if (ImPlot::BeginPlot("##Time", ImVec2(-1,0))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxesLimits(t_min,t_max,0,1);
        if (data != nullptr) {
            // downsample our data
            int downsample = (int)ImPlot::GetPlotLimits().X.Size() / 1000 + 1;
            int start = (int)(ImPlot::GetPlotLimits().X.Min - t_min);
            start = start < 0 ? 0 : start > HugeTimeData::Size - 1 ? HugeTimeData::Size - 1 : start;
            int end = (int)(ImPlot::GetPlotLimits().X.Max - t_min) + 1000;
            end = end < 0 ? 0 : end > HugeTimeData::Size - 1 ? HugeTimeData::Size - 1 : end;
            int size = (end - start)/downsample;
            // plot it
            ImPlot::PlotLine("Time Series", &data->Ts[start], &data->Ys[start], size, 0, 0, sizeof(double)*downsample);
        }
        // plot time now
        double t_now = (double)time(nullptr);
        double y_now = HugeTimeData::GetY(t_now);
        ImPlot::PlotScatter("Now",&t_now,&y_now,1);
        ImPlot::Annotation(t_now,y_now,ImPlot::GetLastItemColor(),ImVec2(10,10),false,"Now");
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

static inline double TransformForward_Sqrt(double v, void*) {
    return sqrt(v);
}

static inline double TransformInverse_Sqrt(double v, void*) {
    return v*v;
}

void Demo_CustomScale() {
    static float v[100];
    for (int i = 0; i < 100; ++i) {
        v[i] = i*0.01f;
    }
    if (ImPlot::BeginPlot("Sqrt")) {
        ImPlot::SetupAxis(ImAxis_X1, "Linear");
        ImPlot::SetupAxis(ImAxis_Y1, "Sqrt");
        ImPlot::SetupAxisScale(ImAxis_Y1, TransformForward_Sqrt, TransformInverse_Sqrt);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, INFINITY);
        ImPlot::PlotLine("##data",v,v,100);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_MultipleAxes() {
    static float xs[1001], xs2[1001], ys1[1001], ys2[1001], ys3[1001];
    for (int i = 0; i < 1001; ++i) {
        xs[i]  = (i*0.1f);
        xs2[i] = xs[i] + 10.0f;
        ys1[i] = sinf(xs[i]) * 3 + 1;
        ys2[i] = cosf(xs[i]) * 0.2f + 0.5f;
        ys3[i] = sinf(xs[i]+0.5f) * 100 + 200;
    }

    static bool x2_axis = true;
    static bool y2_axis = true;
    static bool y3_axis = true;

    ImGui::Checkbox("X-Axis 2", &x2_axis);
    ImGui::SameLine();
    ImGui::Checkbox("Y-Axis 2", &y2_axis);
    ImGui::SameLine();
    ImGui::Checkbox("Y-Axis 3", &y3_axis);

    ImGui::BulletText("You can drag axes to the opposite side of the plot.");
    ImGui::BulletText("Hover over legend items to see which axis they are plotted on.");

    if (ImPlot::BeginPlot("Multi-Axis Plot", ImVec2(-1,0))) {
        ImPlot::SetupAxes("X-Axis 1", "Y-Axis 1");
        ImPlot::SetupAxesLimits(0, 100, 0, 10);
        if (x2_axis) {
            ImPlot::SetupAxis(ImAxis_X2, "X-Axis 2",ImPlotAxisFlags_AuxDefault);
            ImPlot::SetupAxisLimits(ImAxis_X2, 0, 100);
        }
        if (y2_axis) {
            ImPlot::SetupAxis(ImAxis_Y2, "Y-Axis 2",ImPlotAxisFlags_AuxDefault);
            ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 1);
        }
        if (y3_axis) {
            ImPlot::SetupAxis(ImAxis_Y3, "Y-Axis 3",ImPlotAxisFlags_AuxDefault);
            ImPlot::SetupAxisLimits(ImAxis_Y3, 0, 300);
        }

        ImPlot::PlotLine("f(x) = x", xs, xs, 1001);
        if (x2_axis) {
            ImPlot::SetAxes(ImAxis_X2, ImAxis_Y1);
            ImPlot::PlotLine("f(x) = sin(x)*3+1", xs2, ys1, 1001);
        }
        if (y2_axis) {
            ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
            ImPlot::PlotLine("f(x) = cos(x)*.2+.5", xs, ys2, 1001);
        }
        if (x2_axis && y3_axis) {
            ImPlot::SetAxes(ImAxis_X2, ImAxis_Y3);
            ImPlot::PlotLine("f(x) = sin(x+.5)*100+200 ", xs2, ys3, 1001);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_LinkedAxes() {
    static ImPlotRect lims(0,1,0,1);
    static bool linkx = true, linky = true;
    int data[2] = {0,1};
    ImGui::Checkbox("Link X", &linkx);
    ImGui::SameLine();
    ImGui::Checkbox("Link Y", &linky);

    ImGui::DragScalarN("Limits",ImGuiDataType_Double,&lims.X.Min,4,0.01f);

    if (BeginAlignedPlots("AlignedGroup")) {
        if (ImPlot::BeginPlot("Plot A")) {
            ImPlot::SetupAxisLinks(ImAxis_X1, linkx ? &lims.X.Min : nullptr, linkx ? &lims.X.Max : nullptr);
            ImPlot::SetupAxisLinks(ImAxis_Y1, linky ? &lims.Y.Min : nullptr, linky ? &lims.Y.Max : nullptr);
            ImPlot::PlotLine("Line",data,2);
            ImPlot::EndPlot();
        }
        if (ImPlot::BeginPlot("Plot B")) {
            ImPlot::SetupAxisLinks(ImAxis_X1, linkx ? &lims.X.Min : nullptr, linkx ? &lims.X.Max : nullptr);
            ImPlot::SetupAxisLinks(ImAxis_Y1, linky ? &lims.Y.Min : nullptr, linky ? &lims.Y.Max : nullptr);
            ImPlot::PlotLine("Line",data,2);
            ImPlot::EndPlot();
        }
        ImPlot::EndAlignedPlots();
    }
}

//-----------------------------------------------------------------------------

void Demo_AxisConstraints() {
    static float constraints[4] = {-10,10,1,20};
    static ImPlotAxisFlags flags;
    ImGui::DragFloat2("Limits Constraints", &constraints[0], 0.01f);
    ImGui::DragFloat2("Zoom Constraints", &constraints[2], 0.01f);
    CHECKBOX_FLAG(flags, ImPlotAxisFlags_PanStretch);
    if (ImPlot::BeginPlot("##AxisConstraints",ImVec2(-1,0))) {
        ImPlot::SetupAxes("X","Y",flags,flags);
        ImPlot::SetupAxesLimits(-1,1,-1,1);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1,constraints[0], constraints[1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1,constraints[2], constraints[3]);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1,constraints[0], constraints[1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1,constraints[2], constraints[3]);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_EqualAxes() {
    ImGui::BulletText("Equal constraint applies to axis pairs (e.g ImAxis_X1/Y1, ImAxis_X2/Y2)");
    static double xs1[360], ys1[360];
    for (int i = 0; i < 360; ++i) {
        double angle = i * 2 * PI / 359.0;
        xs1[i] = cos(angle); ys1[i] = sin(angle);
    }
    float xs2[] = {-1,0,1,0,-1};
    float ys2[] = {0,1,0,-1,0};
    if (ImPlot::BeginPlot("##EqualAxes",ImVec2(-1,0),ImPlotFlags_Equal)) {
        ImPlot::SetupAxis(ImAxis_X2, nullptr, ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxis(ImAxis_Y2, nullptr, ImPlotAxisFlags_AuxDefault);
        ImPlot::PlotLine("Circle",xs1,ys1,360);
        ImPlot::SetAxes(ImAxis_X2, ImAxis_Y2);
        ImPlot::PlotLine("Diamond",xs2,ys2,5);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_AutoFittingData() {
    ImGui::BulletText("The Y-axis has been configured to auto-fit to only the data visible in X-axis range.");
    ImGui::BulletText("Zoom and pan the X-axis. Disable Stems to see a difference in fit.");
    ImGui::BulletText("If ImPlotAxisFlags_RangeFit is disabled, the axis will fit ALL data.");

    static ImPlotAxisFlags xflags = ImPlotAxisFlags_None;
    static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit;

    ImGui::TextUnformatted("X: "); ImGui::SameLine();
    ImGui::CheckboxFlags("ImPlotAxisFlags_AutoFit##X", (unsigned int*)&xflags, ImPlotAxisFlags_AutoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("ImPlotAxisFlags_RangeFit##X", (unsigned int*)&xflags, ImPlotAxisFlags_RangeFit);

    ImGui::TextUnformatted("Y: "); ImGui::SameLine();
    ImGui::CheckboxFlags("ImPlotAxisFlags_AutoFit##Y", (unsigned int*)&yflags, ImPlotAxisFlags_AutoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("ImPlotAxisFlags_RangeFit##Y", (unsigned int*)&yflags, ImPlotAxisFlags_RangeFit);

    static double data[101];
    srand(0);
    for (int i = 0; i < 101; ++i)
        data[i] = 1 + sin(i/10.0f);

    if (ImPlot::BeginPlot("##DataFitting")) {
        ImPlot::SetupAxes("X","Y",xflags,yflags);
        ImPlot::PlotLine("Line",data,101);
        ImPlot::PlotStems("Stems",data,101);
        ImPlot::EndPlot();
    };
}

//-----------------------------------------------------------------------------

ImPlotPoint SinewaveGetter(int i, void* data) {
    float f = *(float*)data;
    return ImPlotPoint(i,sinf(f*i));
}

void Demo_SubplotsSizing() {

    static ImPlotSubplotFlags flags = ImPlotSubplotFlags_ShareItems|ImPlotSubplotFlags_NoLegend;
    ImGui::CheckboxFlags("ImPlotSubplotFlags_NoResize", (unsigned int*)&flags, ImPlotSubplotFlags_NoResize);
    ImGui::CheckboxFlags("ImPlotSubplotFlags_NoTitle", (unsigned int*)&flags, ImPlotSubplotFlags_NoTitle);

    static int rows = 3;
    static int cols = 3;
    ImGui::SliderInt("Rows",&rows,1,5);
    ImGui::SliderInt("Cols",&cols,1,5);
    if (rows < 1 || cols < 1) {
        ImGui::TextColored(ImVec4(1,0,0,1), "Nice try, but the number of rows and columns must be greater than 0!");
        return;
    }
    static float rratios[] = {5,1,1,1,1,1};
    static float cratios[] = {5,1,1,1,1,1};
    ImGui::DragScalarN("Row Ratios",ImGuiDataType_Float,rratios,rows,0.01f,nullptr);
    ImGui::DragScalarN("Col Ratios",ImGuiDataType_Float,cratios,cols,0.01f,nullptr);
    if (ImPlot::BeginSubplots("My Subplots", rows, cols, ImVec2(-1,400), flags, rratios, cratios)) {
        int id = 0;
        for (int i = 0; i < rows*cols; ++i) {
            if (ImPlot::BeginPlot("",ImVec2(),ImPlotFlags_NoLegend)) {
                ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
                float fi = 0.01f * (i+1);
                if (rows*cols > 1) {
                    ImPlot::SetNextLineStyle(SampleColormap((float)i/(float)(rows*cols-1),ImPlotColormap_Jet));
                }
                char label[16];
                snprintf(label, sizeof(label), "data%d", id++);
                ImPlot::PlotLineG(label,SinewaveGetter,&fi,1000);
                ImPlot::EndPlot();
            }
        }
        ImPlot::EndSubplots();
    }
}

//-----------------------------------------------------------------------------

void Demo_SubplotItemSharing() {
    static ImPlotSubplotFlags flags = ImPlotSubplotFlags_ShareItems;
    ImGui::CheckboxFlags("ImPlotSubplotFlags_ShareItems", (unsigned int*)&flags, ImPlotSubplotFlags_ShareItems);
    ImGui::CheckboxFlags("ImPlotSubplotFlags_ColMajor", (unsigned int*)&flags, ImPlotSubplotFlags_ColMajor);
    ImGui::BulletText("Drag and drop items from the legend onto plots (except for 'common')");
    static int rows = 2;
    static int cols = 3;
    static int id[] = {0,1,2,3,4,5};
    static int curj = -1;
    if (ImPlot::BeginSubplots("##ItemSharing", rows, cols, ImVec2(-1,400), flags)) {
        ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Sort|ImPlotLegendFlags_Horizontal);
        for (int i = 0; i < rows*cols; ++i) {
            if (ImPlot::BeginPlot("")) {
                float fc = 0.01f;
                ImPlot::PlotLineG("common",SinewaveGetter,&fc,1000);
                for (int j = 0; j < 6; ++j) {
                    if (id[j] == i) {
                        char label[8];
                        float fj = 0.01f * (j+2);
                        snprintf(label, sizeof(label), "data%d", j);
                        ImPlot::PlotLineG(label,SinewaveGetter,&fj,1000);
                        if (ImPlot::BeginDragDropSourceItem(label)) {
                            curj = j;
                            ImGui::SetDragDropPayload("MY_DND",nullptr,0);
                            ImPlot::ItemIcon(GetLastItemColor()); ImGui::SameLine();
                            ImGui::TextUnformatted(label);
                            ImPlot::EndDragDropSource();
                        }
                    }
                }
                if (ImPlot::BeginDragDropTargetPlot()) {
                    if (ImGui::AcceptDragDropPayload("MY_DND"))
                        id[curj] = i;
                    ImPlot::EndDragDropTarget();
                }
                ImPlot::EndPlot();
            }
        }
        ImPlot::EndSubplots();
    }
}

//-----------------------------------------------------------------------------

void Demo_SubplotAxisLinking() {
    static ImPlotSubplotFlags flags = ImPlotSubplotFlags_LinkRows | ImPlotSubplotFlags_LinkCols;
    ImGui::CheckboxFlags("ImPlotSubplotFlags_LinkRows", (unsigned int*)&flags, ImPlotSubplotFlags_LinkRows);
    ImGui::CheckboxFlags("ImPlotSubplotFlags_LinkCols", (unsigned int*)&flags, ImPlotSubplotFlags_LinkCols);
    ImGui::CheckboxFlags("ImPlotSubplotFlags_LinkAllX", (unsigned int*)&flags, ImPlotSubplotFlags_LinkAllX);
    ImGui::CheckboxFlags("ImPlotSubplotFlags_LinkAllY", (unsigned int*)&flags, ImPlotSubplotFlags_LinkAllY);

    static int rows = 2;
    static int cols = 2;
    if (ImPlot::BeginSubplots("##AxisLinking", rows, cols, ImVec2(-1,400), flags)) {
        for (int i = 0; i < rows*cols; ++i) {
            if (ImPlot::BeginPlot("")) {
                ImPlot::SetupAxesLimits(0,1000,-1,1);
                float fc = 0.01f;
                ImPlot::PlotLineG("common",SinewaveGetter,&fc,1000);
                ImPlot::EndPlot();
            }
        }
        ImPlot::EndSubplots();
    }
}

//-----------------------------------------------------------------------------

void Demo_LegendOptions() {
    static ImPlotLocation loc = ImPlotLocation_East;
    ImGui::CheckboxFlags("North", (unsigned int*)&loc, ImPlotLocation_North); ImGui::SameLine();
    ImGui::CheckboxFlags("South", (unsigned int*)&loc, ImPlotLocation_South); ImGui::SameLine();
    ImGui::CheckboxFlags("West",  (unsigned int*)&loc, ImPlotLocation_West);  ImGui::SameLine();
    ImGui::CheckboxFlags("East",  (unsigned int*)&loc, ImPlotLocation_East);

    static ImPlotLegendFlags flags = 0;

    CHECKBOX_FLAG(flags, ImPlotLegendFlags_Horizontal);
    CHECKBOX_FLAG(flags, ImPlotLegendFlags_Outside);
    CHECKBOX_FLAG(flags, ImPlotLegendFlags_Sort);

    ImGui::SliderFloat2("LegendPadding", (float*)&GetStyle().LegendPadding, 0.0f, 20.0f, "%.0f");
    ImGui::SliderFloat2("LegendInnerPadding", (float*)&GetStyle().LegendInnerPadding, 0.0f, 10.0f, "%.0f");
    ImGui::SliderFloat2("LegendSpacing", (float*)&GetStyle().LegendSpacing, 0.0f, 5.0f, "%.0f");

    static int num_dummy_items = 25;
    ImGui::SliderInt("Num Dummy Items (Demo Scrolling)", &num_dummy_items, 0, 100);

    if (ImPlot::BeginPlot("##Legend",ImVec2(-1,0))) {
        ImPlot::SetupLegend(loc, flags);
        static MyImPlot::WaveData data1(0.001, 0.2, 4, 0.2);
        static MyImPlot::WaveData data2(0.001, 0.2, 4, 0.4);
        static MyImPlot::WaveData data3(0.001, 0.2, 4, 0.6);
        static MyImPlot::WaveData data4(0.001, 0.2, 4, 0.8);
        static MyImPlot::WaveData data5(0.001, 0.2, 4, 1.0);

        ImPlot::PlotLineG("Item 002", MyImPlot::SawWave, &data1, 1000);         // "Item B" added to legend
        ImPlot::PlotLineG("Item 001##IDText", MyImPlot::SawWave, &data2, 1000);  // "Item A" added to legend, text after ## used for ID only
        ImPlot::PlotLineG("##NotListed", MyImPlot::SawWave, &data3, 1000);     // plotted, but not added to legend
        ImPlot::PlotLineG("Item 003", MyImPlot::SawWave, &data4, 1000);         // "Item C" added to legend
        ImPlot::PlotLineG("Item 003", MyImPlot::SawWave,  &data5, 1000);         // combined with previous "Item C"

        for (int i = 0; i < num_dummy_items; ++i) {
            char label[16];
            snprintf(label, sizeof(label), "Item %03d", i+4);
            ImPlot::PlotDummy(label);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_DragPoints() {
    ImGui::BulletText("Click and drag each point.");
    static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None;
    ImGui::CheckboxFlags("NoCursors", (unsigned int*)&flags, ImPlotDragToolFlags_NoCursors); ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit", (unsigned int*)&flags, ImPlotDragToolFlags_NoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput", (unsigned int*)&flags, ImPlotDragToolFlags_NoInputs);
    ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
    bool clicked[4] = {false, false, false, false};
    bool hovered[4] = {false, false, false, false};
    bool held[4]    = {false, false, false, false};
    if (ImPlot::BeginPlot("##Bezier",ImVec2(-1,0),ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(nullptr,nullptr,ax_flags,ax_flags);
        ImPlot::SetupAxesLimits(0,1,0,1);
        static ImPlotPoint P[] = {ImPlotPoint(.05f,.05f), ImPlotPoint(0.2,0.4),  ImPlotPoint(0.8,0.6),  ImPlotPoint(.95f,.95f)};

        ImPlot::DragPoint(0,&P[0].x,&P[0].y, ImVec4(0,0.9f,0,1),4,flags, &clicked[0], &hovered[0], &held[0]);
        ImPlot::DragPoint(1,&P[1].x,&P[1].y, ImVec4(1,0.5f,1,1),4,flags, &clicked[1], &hovered[1], &held[1]);
        ImPlot::DragPoint(2,&P[2].x,&P[2].y, ImVec4(0,0.5f,1,1),4,flags, &clicked[2], &hovered[2], &held[2]);
        ImPlot::DragPoint(3,&P[3].x,&P[3].y, ImVec4(0,0.9f,0,1),4,flags, &clicked[3], &hovered[3], &held[3]);

        static ImPlotPoint B[100];
        for (int i = 0; i < 100; ++i) {
            double t  = i / 99.0;
            double u  = 1 - t;
            double w1 = u*u*u;
            double w2 = 3*u*u*t;
            double w3 = 3*u*t*t;
            double w4 = t*t*t;
            B[i] = ImPlotPoint(w1*P[0].x + w2*P[1].x + w3*P[2].x + w4*P[3].x, w1*P[0].y + w2*P[1].y + w3*P[2].y + w4*P[3].y);
        }

        ImPlot::SetNextLineStyle(ImVec4(1,0.5f,1,1),hovered[1]||held[1] ? 2.0f : 1.0f);
        ImPlot::PlotLine("##h1",&P[0].x, &P[0].y, 2, 0, 0, sizeof(ImPlotPoint));
        ImPlot::SetNextLineStyle(ImVec4(0,0.5f,1,1), hovered[2]||held[2] ? 2.0f : 1.0f);
        ImPlot::PlotLine("##h2",&P[2].x, &P[2].y, 2, 0, 0, sizeof(ImPlotPoint));
        ImPlot::SetNextLineStyle(ImVec4(0,0.9f,0,1), hovered[0]||held[0]||hovered[3]||held[3] ? 3.0f : 2.0f);
        ImPlot::PlotLine("##bez",&B[0].x, &B[0].y, 100, 0, 0, sizeof(ImPlotPoint));
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_DragLines() {
    ImGui::BulletText("Click and drag the horizontal and vertical lines.");
    static double x1 = 0.2;
    static double x2 = 0.8;
    static double y1 = 0.25;
    static double y2 = 0.75;
    static double f = 0.1;
    bool clicked = false;
    bool hovered = false;
    bool held = false;
    static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None;
    ImGui::CheckboxFlags("NoCursors", (unsigned int*)&flags, ImPlotDragToolFlags_NoCursors); ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit", (unsigned int*)&flags, ImPlotDragToolFlags_NoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput", (unsigned int*)&flags, ImPlotDragToolFlags_NoInputs);
    if (ImPlot::BeginPlot("##lines",ImVec2(-1,0))) {
        ImPlot::SetupAxesLimits(0,1,0,1);
        ImPlot::DragLineX(0,&x1,ImVec4(1,1,1,1),1,flags);
        ImPlot::DragLineX(1,&x2,ImVec4(1,1,1,1),1,flags);
        ImPlot::DragLineY(2,&y1,ImVec4(1,1,1,1),1,flags);
        ImPlot::DragLineY(3,&y2,ImVec4(1,1,1,1),1,flags);
        double xs[1000], ys[1000];
        for (int i = 0; i < 1000; ++i) {
            xs[i] = (x2+x1)/2+fabs(x2-x1)*(i/1000.0f - 0.5f);
            ys[i] = (y1+y2)/2+fabs(y2-y1)/2*sin(f*i/10);
        }
        ImPlot::DragLineY(120482,&f,ImVec4(1,0.5f,1,1),1,flags, &clicked, &hovered, &held);
        ImPlot::SetNextLineStyle(IMPLOT_AUTO_COL, hovered||held ? 2.0f : 1.0f);
        ImPlot::PlotLine("Interactive Data", xs, ys, 1000);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_DragRects() {

    static float x_data[512];
    static float y_data1[512];
    static float y_data2[512];
    static float y_data3[512];
    static float sampling_freq = 44100;
    static float freq = 500;
    bool clicked = false;
    bool hovered = false;
    bool held = false;
    for (size_t i = 0; i < 512; ++i) {
        const float t = i / sampling_freq;
        x_data[i] = t;
        const float arg = 2 * 3.14f * freq * t;
        y_data1[i] = sinf(arg);
        y_data2[i] = y_data1[i] * -0.6f + sinf(2 * arg) * 0.4f;
        y_data3[i] = y_data2[i] * -0.6f + sinf(3 * arg) * 0.4f;
    }
    ImGui::BulletText("Click and drag the edges, corners, and center of the rect.");
    ImGui::BulletText("Double click edges to expand rect to plot extents.");
    static ImPlotRect rect(0.0025,0.0045,0,0.5);
    static ImPlotDragToolFlags flags = ImPlotDragToolFlags_None;
    ImGui::CheckboxFlags("NoCursors", (unsigned int*)&flags, ImPlotDragToolFlags_NoCursors); ImGui::SameLine();
    ImGui::CheckboxFlags("NoFit", (unsigned int*)&flags, ImPlotDragToolFlags_NoFit); ImGui::SameLine();
    ImGui::CheckboxFlags("NoInput", (unsigned int*)&flags, ImPlotDragToolFlags_NoInputs);

    if (ImPlot::BeginPlot("##Main",ImVec2(-1,150))) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoTickLabels,ImPlotAxisFlags_NoTickLabels);
        ImPlot::SetupAxesLimits(0,0.01,-1,1);
        ImPlot::PlotLine("Signal 1", x_data, y_data1, 512);
        ImPlot::PlotLine("Signal 2", x_data, y_data2, 512);
        ImPlot::PlotLine("Signal 3", x_data, y_data3, 512);
        ImPlot::DragRect(0,&rect.X.Min,&rect.Y.Min,&rect.X.Max,&rect.Y.Max,ImVec4(1,0,1,1),flags, &clicked, &hovered, &held);
        ImPlot::EndPlot();
    }
    ImVec4 bg_col = held ? ImVec4(0.5f,0,0.5f,1) : (hovered ? ImVec4(0.25f,0,0.25f,1) : ImPlot::GetStyle().Colors[ImPlotCol_PlotBg]);
    ImPlot::PushStyleColor(ImPlotCol_PlotBg, bg_col);
    if (ImPlot::BeginPlot("##rect",ImVec2(-1,150), ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(rect.X.Min, rect.X.Max, rect.Y.Min, rect.Y.Max, ImGuiCond_Always);
        ImPlot::PlotLine("Signal 1", x_data, y_data1, 512);
        ImPlot::PlotLine("Signal 2", x_data, y_data2, 512);
        ImPlot::PlotLine("Signal 3", x_data, y_data3, 512);
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleColor();
    ImGui::Text("Rect is %sclicked, %shovered, %sheld", clicked ? "" : "not ", hovered ? "" : "not ", held ? "" : "not ");
}

//-----------------------------------------------------------------------------

ImPlotPoint FindCentroid(const ImVector<ImPlotPoint>& data, const ImPlotRect& bounds, int& cnt) {
    cnt = 0;
    ImPlotPoint avg;
    ImPlotRect bounds_fixed;
    bounds_fixed.X.Min = bounds.X.Min < bounds.X.Max ? bounds.X.Min : bounds.X.Max;
    bounds_fixed.X.Max = bounds.X.Min < bounds.X.Max ? bounds.X.Max : bounds.X.Min;
    bounds_fixed.Y.Min = bounds.Y.Min < bounds.Y.Max ? bounds.Y.Min : bounds.Y.Max;
    bounds_fixed.Y.Max = bounds.Y.Min < bounds.Y.Max ? bounds.Y.Max : bounds.Y.Min;
    for (int i = 0; i < data.size(); ++i) {
        if (bounds_fixed.Contains(data[i].x, data[i].y)) {
            avg.x += data[i].x;
            avg.y += data[i].y;
            cnt++;
        }
    }
    if (cnt > 0) {
        avg.x = avg.x / cnt;
        avg.y = avg.y / cnt;
    }
    return avg;
}

//-----------------------------------------------------------------------------

void Demo_Querying() {
    static ImVector<ImPlotPoint> data;
    static ImVector<ImPlotRect> rects;
    static ImPlotRect limits, select;
    static bool init = true;
    if (init) {
        for (int i = 0; i < 50; ++i)
        {
            double x = RandomRange(0.1, 0.9);
            double y = RandomRange(0.1, 0.9);
            data.push_back(ImPlotPoint(x,y));
        }
        init = false;
    }

    ImGui::BulletText("Box select and left click mouse to create a new query rect.");
    ImGui::BulletText("Ctrl + click in the plot area to draw points.");

    if (ImGui::Button("Clear Queries"))
        rects.shrink(0);

    if (ImPlot::BeginPlot("##Centroid")) {
        ImPlot::SetupAxesLimits(0,1,0,1);
        if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl) {
            ImPlotPoint pt = ImPlot::GetPlotMousePos();
            data.push_back(pt);
        }
        ImPlot::PlotScatter("Points", &data[0].x, &data[0].y, data.size(), 0, 0, 2 * sizeof(double));
        if (ImPlot::IsPlotSelected()) {
            select = ImPlot::GetPlotSelection();
            int cnt;
            ImPlotPoint centroid = FindCentroid(data,select,cnt);
            if (cnt > 0) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square,6);
                ImPlot::PlotScatter("Centroid", &centroid.x, &centroid.y, 1);
            }
            if (ImGui::IsMouseClicked(ImPlot::GetInputMap().SelectCancel)) {
                CancelPlotSelection();
                rects.push_back(select);
            }
        }
        for (int i = 0; i < rects.size(); ++i) {
            int cnt;
            ImPlotPoint centroid = FindCentroid(data,rects[i],cnt);
            if (cnt > 0) {
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Square,6);
                ImPlot::PlotScatter("Centroid", &centroid.x, &centroid.y, 1);
            }
            ImPlot::DragRect(i,&rects[i].X.Min,&rects[i].Y.Min,&rects[i].X.Max,&rects[i].Y.Max,ImVec4(1,0,1,1));
        }
        limits  = ImPlot::GetPlotLimits();
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_Annotations() {
    static bool clamp = false;
    ImGui::Checkbox("Clamp",&clamp);
    if (ImPlot::BeginPlot("##Annotations")) {
        ImPlot::SetupAxesLimits(0,2,0,1);
        static float p[] = {0.25f, 0.25f, 0.75f, 0.75f, 0.25f};
        ImPlot::PlotScatter("##Points",&p[0],&p[1],4);
        ImVec4 col = GetLastItemColor();
        ImPlot::Annotation(0.25,0.25,col,ImVec2(-15,15),clamp,"BL");
        ImPlot::Annotation(0.75,0.25,col,ImVec2(15,15),clamp,"BR");
        ImPlot::Annotation(0.75,0.75,col,ImVec2(15,-15),clamp,"TR");
        ImPlot::Annotation(0.25,0.75,col,ImVec2(-15,-15),clamp,"TL");
        ImPlot::Annotation(0.5,0.5,col,ImVec2(0,0),clamp,"Center");

        ImPlot::Annotation(1.25,0.75,ImVec4(0,1,0,1),ImVec2(0,0),clamp);

        float bx[] = {1.2f,1.5f,1.8f};
        float by[] = {0.25f, 0.5f, 0.75f};
        ImPlot::PlotBars("##Bars",bx,by,3,0.2);
        for (int i = 0; i < 3; ++i)
            ImPlot::Annotation(bx[i],by[i],ImVec4(0,0,0,0),ImVec2(0,-5),clamp,"B[%d]=%.2f",i,by[i]);
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_Tags() {
    static bool show = true;
    ImGui::Checkbox("Show Tags",&show);
    if (ImPlot::BeginPlot("##Tags")) {
        ImPlot::SetupAxis(ImAxis_X2);
        ImPlot::SetupAxis(ImAxis_Y2);
        if (show) {
            ImPlot::TagX(0.25, ImVec4(1,1,0,1));
            ImPlot::TagY(0.75, ImVec4(1,1,0,1));
            static double drag_tag = 0.25;
            ImPlot::DragLineY(0,&drag_tag,ImVec4(1,0,0,1),1,ImPlotDragToolFlags_NoFit);
            ImPlot::TagY(drag_tag, ImVec4(1,0,0,1), "Drag");
            SetAxes(ImAxis_X2, ImAxis_Y2);
            ImPlot::TagX(0.5, ImVec4(0,1,1,1), "%s", "MyTag");
            ImPlot::TagY(0.5, ImVec4(0,1,1,1), "Tag: %d", 42);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_DragAndDrop() {
    ImGui::BulletText("Drag/drop items from the left column.");
    ImGui::BulletText("Drag/drop items between plots.");
    ImGui::Indent();
    ImGui::BulletText("Plot 1 Targets: Plot, Y-Axes, Legend");
    ImGui::BulletText("Plot 1 Sources: Legend Item Labels");
    ImGui::BulletText("Plot 2 Targets: Plot, X-Axis, Y-Axis");
    ImGui::BulletText("Plot 2 Sources: Plot, X-Axis, Y-Axis (hold Ctrl)");
    ImGui::Unindent();

    // convenience struct to manage DND items; do this however you like
    struct MyDndItem {
        int              Idx;
        int              Plt;
        ImAxis           Yax;
        char             Label[16];
        ImVector<ImVec2> Data;
        ImVec4           Color;
        MyDndItem()        {
            static int i = 0;
            Idx = i++;
            Plt = 0;
            Yax = ImAxis_Y1;
            snprintf(Label, sizeof(Label), "%02d Hz", Idx+1);
            Color = RandomColor();
            Data.reserve(1001);
            for (int k = 0; k < 1001; ++k) {
                float t = k * 1.0f / 999;
                Data.push_back(ImVec2(t, 0.5f + 0.5f * sinf(2*3.14f*t*(Idx+1))));
            }
        }
        void Reset() { Plt = 0; Yax = ImAxis_Y1; }
    };

    const int         k_dnd = 20;
    static MyDndItem  dnd[k_dnd];
    static MyDndItem* dndx = nullptr; // for plot 2
    static MyDndItem* dndy = nullptr; // for plot 2

    // child window to serve as initial source for our DND items
    ImGui::BeginChild("DND_LEFT",ImVec2(100,400));
    if (ImGui::Button("Reset Data")) {
        for (int k = 0; k < k_dnd; ++k)
            dnd[k].Reset();
        dndx = dndy = nullptr;
    }
    for (int k = 0; k < k_dnd; ++k) {
        if (dnd[k].Plt > 0)
            continue;
        ImPlot::ItemIcon(dnd[k].Color); ImGui::SameLine();
        ImGui::Selectable(dnd[k].Label, false, 0, ImVec2(100, 0));
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("MY_DND", &k, sizeof(int));
            ImPlot::ItemIcon(dnd[k].Color); ImGui::SameLine();
            ImGui::TextUnformatted(dnd[k].Label);
            ImGui::EndDragDropSource();
        }
    }
    ImGui::EndChild();
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
            int i = *(int*)payload->Data; dnd[i].Reset();
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::SameLine();
    ImGui::BeginChild("DND_RIGHT",ImVec2(-1,400));
    // plot 1 (time series)
    ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoHighlight;
    if (ImPlot::BeginPlot("##DND1", ImVec2(-1,195))) {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, flags|ImPlotAxisFlags_Lock);
        ImPlot::SetupAxis(ImAxis_Y1, "[drop here]", flags);
        ImPlot::SetupAxis(ImAxis_Y2, "[drop here]", flags|ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxis(ImAxis_Y3, "[drop here]", flags|ImPlotAxisFlags_Opposite);

        for (int k = 0; k < k_dnd; ++k) {
            if (dnd[k].Plt == 1 && dnd[k].Data.size() > 0) {
                ImPlot::SetAxis(dnd[k].Yax);
                ImPlot::SetNextLineStyle(dnd[k].Color);
                ImPlot::PlotLine(dnd[k].Label, &dnd[k].Data[0].x, &dnd[k].Data[0].y, dnd[k].Data.size(), 0, 0, 2 * sizeof(float));
                // allow legend item labels to be DND sources
                if (ImPlot::BeginDragDropSourceItem(dnd[k].Label)) {
                    ImGui::SetDragDropPayload("MY_DND", &k, sizeof(int));
                    ImPlot::ItemIcon(dnd[k].Color); ImGui::SameLine();
                    ImGui::TextUnformatted(dnd[k].Label);
                    ImPlot::EndDragDropSource();
                }
            }
        }
        // allow the main plot area to be a DND target
        if (ImPlot::BeginDragDropTargetPlot()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
                int i = *(int*)payload->Data; dnd[i].Plt = 1; dnd[i].Yax = ImAxis_Y1;
            }
            ImPlot::EndDragDropTarget();
        }
        // allow each y-axis to be a DND target
        for (int y = ImAxis_Y1; y <= ImAxis_Y3; ++y) {
            if (ImPlot::BeginDragDropTargetAxis(y)) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
                    int i = *(int*)payload->Data; dnd[i].Plt = 1; dnd[i].Yax = y;
                }
                ImPlot::EndDragDropTarget();
            }
        }
        // allow the legend to be a DND target
        if (ImPlot::BeginDragDropTargetLegend()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
                int i = *(int*)payload->Data; dnd[i].Plt = 1; dnd[i].Yax = ImAxis_Y1;
            }
            ImPlot::EndDragDropTarget();
        }
        ImPlot::EndPlot();
    }
    // plot 2 (Lissajous)
    if (ImPlot::BeginPlot("##DND2", ImVec2(-1,195))) {
        ImPlot::PushStyleColor(ImPlotCol_AxisBg, dndx != nullptr ? dndx->Color : ImPlot::GetStyle().Colors[ImPlotCol_AxisBg]);
        ImPlot::SetupAxis(ImAxis_X1, dndx == nullptr ? "[drop here]" : dndx->Label, flags);
        ImPlot::PushStyleColor(ImPlotCol_AxisBg, dndy != nullptr ? dndy->Color : ImPlot::GetStyle().Colors[ImPlotCol_AxisBg]);
        ImPlot::SetupAxis(ImAxis_Y1, dndy == nullptr ? "[drop here]" : dndy->Label, flags);
        ImPlot::PopStyleColor(2);
        if (dndx != nullptr && dndy != nullptr) {
            ImVec4 mixed((dndx->Color.x + dndy->Color.x)/2,(dndx->Color.y + dndy->Color.y)/2,(dndx->Color.z + dndy->Color.z)/2,(dndx->Color.w + dndy->Color.w)/2);
            ImPlot::SetNextLineStyle(mixed);
            ImPlot::PlotLine("##dndxy", &dndx->Data[0].y, &dndy->Data[0].y, dndx->Data.size(), 0, 0, 2 * sizeof(float));
        }
        // allow the x-axis to be a DND target
        if (ImPlot::BeginDragDropTargetAxis(ImAxis_X1)) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
                int i = *(int*)payload->Data; dndx = &dnd[i];
            }
            ImPlot::EndDragDropTarget();
        }
        // allow the x-axis to be a DND source
        if (dndx != nullptr && ImPlot::BeginDragDropSourceAxis(ImAxis_X1)) {
            ImGui::SetDragDropPayload("MY_DND", &dndx->Idx, sizeof(int));
            ImPlot::ItemIcon(dndx->Color); ImGui::SameLine();
            ImGui::TextUnformatted(dndx->Label);
            ImPlot::EndDragDropSource();
        }
        // allow the y-axis to be a DND target
        if (ImPlot::BeginDragDropTargetAxis(ImAxis_Y1)) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
                int i = *(int*)payload->Data; dndy = &dnd[i];
            }
            ImPlot::EndDragDropTarget();
        }
        // allow the y-axis to be a DND source
        if (dndy != nullptr && ImPlot::BeginDragDropSourceAxis(ImAxis_Y1)) {
            ImGui::SetDragDropPayload("MY_DND", &dndy->Idx, sizeof(int));
            ImPlot::ItemIcon(dndy->Color); ImGui::SameLine();
            ImGui::TextUnformatted(dndy->Label);
            ImPlot::EndDragDropSource();
        }
        // allow the plot area to be a DND target
        if (ImPlot::BeginDragDropTargetPlot()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MY_DND")) {
                int i = *(int*)payload->Data; dndx = dndy = &dnd[i];
            }
        }
        // allow the plot area to be a DND source
        if (ImPlot::BeginDragDropSourcePlot()) {
            ImGui::TextUnformatted("Yes, you can\ndrag this!");
            ImPlot::EndDragDropSource();
        }
        ImPlot::EndPlot();
    }
    ImGui::EndChild();
}

//-----------------------------------------------------------------------------

void Demo_Tables() {
#ifdef IMGUI_HAS_TABLE
    static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                   ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
    static bool anim = true;
    static int offset = 0;
    ImGui::BulletText("Plots can be used inside of ImGui tables as another means of creating subplots.");
    ImGui::Checkbox("Animate",&anim);
    if (anim)
        offset = (offset + 1) % 100;
    if (ImGui::BeginTable("##table", 3, flags, ImVec2(-1,0))) {
        ImGui::TableSetupColumn("Electrode", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("Voltage", ImGuiTableColumnFlags_WidthFixed, 75.0f);
        ImGui::TableSetupColumn("EMG Signal");
        ImGui::TableHeadersRow();
        ImPlot::PushColormap(ImPlotColormap_Cool);
        for (int row = 0; row < 10; row++) {
            ImGui::TableNextRow();
            static float data[100];
            srand(row);
            for (int i = 0; i < 100; ++i)
                data[i] = RandomRange(0.0f,10.0f);
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("EMG %d", row);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f V", data[offset]);
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(row);
            MyImPlot::Sparkline("##spark",data,100,0,11.0f,offset,ImPlot::GetColormapColor(row),ImVec2(-1, 35));
            ImGui::PopID();
        }
        ImPlot::PopColormap();
        ImGui::EndTable();
    }
#else
    ImGui::BulletText("You need to merge the ImGui 'tables' branch for this section.");
#endif
}

//-----------------------------------------------------------------------------

void Demo_OffsetAndStride() {
    static const int k_circles    = 11;
    static const int k_points_per = 50;
    static const int k_size       = 2 * k_points_per * k_circles;
    static double interleaved_data[k_size];
    for (int p = 0; p < k_points_per; ++p) {
        for (int c = 0; c < k_circles; ++c) {
            double r = (double)c / (k_circles - 1) * 0.2 + 0.2;
            interleaved_data[p*2*k_circles + 2*c + 0] = 0.5 + r * cos((double)p/k_points_per * 6.28);
            interleaved_data[p*2*k_circles + 2*c + 1] = 0.5 + r * sin((double)p/k_points_per * 6.28);
        }
    }
    static int offset = 0;
    ImGui::BulletText("Offsetting is useful for realtime plots (see above) and circular buffers.");
    ImGui::BulletText("Striding is useful for interleaved data (e.g. audio) or plotting structs.");
    ImGui::BulletText("Here, all circle data is stored in a single interleaved buffer:");
    ImGui::BulletText("[c0.x0 c0.y0 ... cn.x0 cn.y0 c0.x1 c0.y1 ... cn.x1 cn.y1 ... cn.xm cn.ym]");
    ImGui::BulletText("The offset value indicates which circle point index is considered the first.");
    ImGui::BulletText("Offsets can be negative and/or larger than the actual data count.");
    ImGui::SliderInt("Offset", &offset, -2*k_points_per, 2*k_points_per);
    if (ImPlot::BeginPlot("##strideoffset",ImVec2(-1,0),ImPlotFlags_Equal)) {
        ImPlot::PushColormap(ImPlotColormap_Jet);
        char buff[32];
        for (int c = 0; c < k_circles; ++c) {
            snprintf(buff, sizeof(buff), "Circle %d", c);
            ImPlot::PlotLine(buff, &interleaved_data[c*2 + 0], &interleaved_data[c*2 + 1], k_points_per, 0, offset, 2*k_circles*sizeof(double));
        }
        ImPlot::EndPlot();
        ImPlot::PopColormap();
    }
    // offset++; uncomment for animation!
}

//-----------------------------------------------------------------------------

void Demo_CustomDataAndGetters() {
    ImGui::BulletText("You can plot custom structs using the stride feature.");
    ImGui::BulletText("Most plotters can also be passed a function pointer for getting data.");
    ImGui::Indent();
        ImGui::BulletText("You can optionally pass user data to be given to your getter function.");
        ImGui::BulletText("C++ lambdas can be passed as function pointers as well!");
    ImGui::Unindent();

    MyImPlot::Vector2f vec2_data[2] = { MyImPlot::Vector2f(0,0), MyImPlot::Vector2f(1,1) };

    if (ImPlot::BeginPlot("##Custom Data")) {

        // custom structs using stride example:
        ImPlot::PlotLine("Vector2f", &vec2_data[0].x, &vec2_data[0].y, 2, 0, 0, sizeof(MyImPlot::Vector2f) /* or sizeof(float) * 2 */);

        // custom getter example 1:
        ImPlot::PlotLineG("Spiral", MyImPlot::Spiral, nullptr, 1000);

        // custom getter example 2:
        static MyImPlot::WaveData data1(0.001, 0.2, 2, 0.75);
        static MyImPlot::WaveData data2(0.001, 0.2, 4, 0.25);
        ImPlot::PlotLineG("Waves", MyImPlot::SineWave, &data1, 1000);
        ImPlot::PlotLineG("Waves", MyImPlot::SawWave, &data2, 1000);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShadedG("Waves", MyImPlot::SineWave, &data1, MyImPlot::SawWave, &data2, 1000);
        ImPlot::PopStyleVar();

        // you can also pass C++ lambdas:
        // auto lamda = [](void* data, int idx) { ... return ImPlotPoint(x,y); };
        // ImPlot::PlotLine("My Lambda", lambda, data, 1000);

        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

int MetricFormatter(double value, char* buff, int size, void* data) {
    const char* unit = (const char*)data;
    static double v[]      = {1000000000,1000000,1000,1,0.001,0.000001,0.000000001};
    static const char* p[] = {"G","M","k","","m","u","n"};
    if (value == 0) {
        return snprintf(buff,size,"0 %s", unit);
    }
    for (int i = 0; i < 7; ++i) {
        if (fabs(value) >= v[i]) {
            return snprintf(buff,size,"%g %s%s",value/v[i],p[i],unit);
        }
    }
    return snprintf(buff,size,"%g %s%s",value/v[6],p[6],unit);
}

void Demo_TickLabels()  {
    static bool custom_fmt    = true;
    static bool custom_ticks  = false;
    static bool custom_labels = true;
    ImGui::Checkbox("Show Custom Format", &custom_fmt);
    ImGui::SameLine();
    ImGui::Checkbox("Show Custom Ticks", &custom_ticks);
    if (custom_ticks) {
        ImGui::SameLine();
        ImGui::Checkbox("Show Custom Labels", &custom_labels);
    }
    const double pi = 3.14;
    const char* pi_str[] = {"PI"};
    static double yticks[] = {100,300,700,900};
    static const char*  ylabels[] = {"One","Three","Seven","Nine"};
    static double yticks_aux[] = {0.2,0.4,0.6};
    static const char* ylabels_aux[] = {"A","B","C","D","E","F"};
    if (ImPlot::BeginPlot("##Ticks")) {
        ImPlot::SetupAxesLimits(2.5,5,0,1000);
        ImPlot::SetupAxis(ImAxis_Y2, nullptr, ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxis(ImAxis_Y3, nullptr, ImPlotAxisFlags_AuxDefault);
        if (custom_fmt) {
            ImPlot::SetupAxisFormat(ImAxis_X1, "%g ms");
            ImPlot::SetupAxisFormat(ImAxis_Y1, MetricFormatter, (void*)"Hz");
            ImPlot::SetupAxisFormat(ImAxis_Y2, "%g dB");
            ImPlot::SetupAxisFormat(ImAxis_Y3, MetricFormatter, (void*)"m");
        }
        if (custom_ticks) {
            ImPlot::SetupAxisTicks(ImAxis_X1, &pi,1,custom_labels ? pi_str : nullptr, true);
            ImPlot::SetupAxisTicks(ImAxis_Y1, yticks, 4, custom_labels ? ylabels : nullptr, false);
            ImPlot::SetupAxisTicks(ImAxis_Y2, yticks_aux, 3, custom_labels ? ylabels_aux : nullptr, false);
            ImPlot::SetupAxisTicks(ImAxis_Y3, 0, 1, 6, custom_labels ? ylabels_aux : nullptr, false);
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_CustomStyles() {
    ImPlot::PushColormap(ImPlotColormap_Deep);
    // normally you wouldn't change the entire style each frame
    ImPlotStyle backup = ImPlot::GetStyle();
    MyImPlot::StyleSeaborn();
    if (ImPlot::BeginPlot("seaborn style")) {
        ImPlot::SetupAxes( "x-axis", "y-axis");
        ImPlot::SetupAxesLimits(-0.5f, 9.5f, 0, 10);
        unsigned int lin[10] = {8,8,9,7,8,8,8,9,7,8};
        unsigned int bar[10] = {1,2,5,3,4,1,2,5,3,4};
        unsigned int dot[10] = {7,6,6,7,8,5,6,5,8,7};
        ImPlot::PlotBars("Bars", bar, 10, 0.5f);
        ImPlot::PlotLine("Line", lin, 10);
        ImPlot::NextColormapColor(); // skip green
        ImPlot::PlotScatter("Scatter", dot, 10);
        ImPlot::EndPlot();
    }
    ImPlot::GetStyle() = backup;
    ImPlot::PopColormap();
}

//-----------------------------------------------------------------------------

void Demo_CustomRendering() {
    if (ImPlot::BeginPlot("##CustomRend")) {
        ImVec2 cntr = ImPlot::PlotToPixels(ImPlotPoint(0.5f,  0.5f));
        ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(0.25f, 0.75f));
        ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(0.75f, 0.25f));
        ImPlot::PushPlotClipRect();
        ImPlot::GetPlotDrawList()->AddCircleFilled(cntr,20,IM_COL32(255,255,0,255),20);
        ImPlot::GetPlotDrawList()->AddRect(rmin, rmax, IM_COL32(128,0,255,255));
        ImPlot::PopPlotClipRect();
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_LegendPopups() {
    ImGui::BulletText("You can implement legend context menus to inject per-item controls and widgets.");
    ImGui::BulletText("Right click the legend label/icon to edit custom item attributes.");

    static float  frequency = 0.1f;
    static float  amplitude = 0.5f;
    static ImVec4 color     = ImVec4(1,1,0,1);
    static float  alpha     = 1.0f;
    static bool   line      = false;
    static float  thickness = 1;
    static bool   markers   = false;
    static bool   shaded    = false;

    static float vals[101];
    for (int i = 0; i < 101; ++i)
        vals[i] = amplitude * sinf(frequency * i);

    if (ImPlot::BeginPlot("Right Click the Legend")) {
        ImPlot::SetupAxesLimits(0,100,-1,1);
        // rendering logic
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, alpha);
        if (!line) {
            ImPlot::SetNextFillStyle(color);
            ImPlot::PlotBars("Right Click Me", vals, 101);
        }
        else {
            if (markers) ImPlot::SetNextMarkerStyle(ImPlotMarker_Square);
            ImPlot::SetNextLineStyle(color, thickness);
            ImPlot::PlotLine("Right Click Me", vals, 101);
            if (shaded) ImPlot::PlotShaded("Right Click Me",vals,101);
        }
        ImPlot::PopStyleVar();
        // custom legend context menu
        if (ImPlot::BeginLegendPopup("Right Click Me")) {
            ImGui::SliderFloat("Frequency",&frequency,0,1,"%0.2f");
            ImGui::SliderFloat("Amplitude",&amplitude,0,1,"%0.2f");
            ImGui::Separator();
            ImGui::ColorEdit3("Color",&color.x);
            ImGui::SliderFloat("Transparency",&alpha,0,1,"%.2f");
            ImGui::Checkbox("Line Plot", &line);
            if (line) {
                ImGui::SliderFloat("Thickness", &thickness, 0, 5);
                ImGui::Checkbox("Markers", &markers);
                ImGui::Checkbox("Shaded",&shaded);
            }
            ImPlot::EndLegendPopup();
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_ColormapWidgets() {
    static int cmap = ImPlotColormap_Viridis;

    if (ImPlot::ColormapButton("Button",ImVec2(0,0),cmap)) {
        cmap = (cmap + 1) % ImPlot::GetColormapCount();
    }

    static float t = 0.5f;
    static ImVec4 col;
    ImGui::ColorButton("##Display",col,ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    ImPlot::ColormapSlider("Slider", &t, &col, "%.3f", cmap);

    ImPlot::ColormapIcon(cmap); ImGui::SameLine(); ImGui::Text("Icon");

    static ImPlotColormapScaleFlags flags = 0;
    static float scale[2] = {0, 100};
    ImPlot::ColormapScale("Scale",scale[0],scale[1],ImVec2(0,0),"%g dB",flags,cmap);
    ImGui::InputFloat2("Scale",scale);
    CHECKBOX_FLAG(flags, ImPlotColormapScaleFlags_NoLabel);
    CHECKBOX_FLAG(flags, ImPlotColormapScaleFlags_Opposite);
    CHECKBOX_FLAG(flags, ImPlotColormapScaleFlags_Invert);
}

//-----------------------------------------------------------------------------

void Demo_CustomPlottersAndTooltips()  {
    ImGui::BulletText("You can create custom plotters or extend ImPlot using implot_internal.h.");
    double dates[]  = {1546300800,1546387200,1546473600,1546560000,1546819200,1546905600,1546992000,1547078400,1547164800,1547424000,1547510400,1547596800,1547683200,1547769600,1547942400,1548028800,1548115200,1548201600,1548288000,1548374400,1548633600,1548720000,1548806400,1548892800,1548979200,1549238400,1549324800,1549411200,1549497600,1549584000,1549843200,1549929600,1550016000,1550102400,1550188800,1550361600,1550448000,1550534400,1550620800,1550707200,1550793600,1551052800,1551139200,1551225600,1551312000,1551398400,1551657600,1551744000,1551830400,1551916800,1552003200,1552262400,1552348800,1552435200,1552521600,1552608000,1552867200,1552953600,1553040000,1553126400,1553212800,1553472000,1553558400,1553644800,1553731200,1553817600,1554076800,1554163200,1554249600,1554336000,1554422400,1554681600,1554768000,1554854400,1554940800,1555027200,1555286400,1555372800,1555459200,1555545600,1555632000,1555891200,1555977600,1556064000,1556150400,1556236800,1556496000,1556582400,1556668800,1556755200,1556841600,1557100800,1557187200,1557273600,1557360000,1557446400,1557705600,1557792000,1557878400,1557964800,1558051200,1558310400,1558396800,1558483200,1558569600,1558656000,1558828800,1558915200,1559001600,1559088000,1559174400,1559260800,1559520000,1559606400,1559692800,1559779200,1559865600,1560124800,1560211200,1560297600,1560384000,1560470400,1560729600,1560816000,1560902400,1560988800,1561075200,1561334400,1561420800,1561507200,1561593600,1561680000,1561939200,1562025600,1562112000,1562198400,1562284800,1562544000,1562630400,1562716800,1562803200,1562889600,1563148800,1563235200,1563321600,1563408000,1563494400,1563753600,1563840000,1563926400,1564012800,1564099200,1564358400,1564444800,1564531200,1564617600,1564704000,1564963200,1565049600,1565136000,1565222400,1565308800,1565568000,1565654400,1565740800,1565827200,1565913600,1566172800,1566259200,1566345600,1566432000,1566518400,1566777600,1566864000,1566950400,1567036800,1567123200,1567296000,1567382400,1567468800,1567555200,1567641600,1567728000,1567987200,1568073600,1568160000,1568246400,1568332800,1568592000,1568678400,1568764800,1568851200,1568937600,1569196800,1569283200,1569369600,1569456000,1569542400,1569801600,1569888000,1569974400,1570060800,1570147200,1570406400,1570492800,1570579200,1570665600,1570752000,1571011200,1571097600,1571184000,1571270400,1571356800,1571616000,1571702400,1571788800,1571875200,1571961600};
    double opens[]  = {1284.7,1319.9,1318.7,1328,1317.6,1321.6,1314.3,1325,1319.3,1323.1,1324.7,1321.3,1323.5,1322,1281.3,1281.95,1311.1,1315,1314,1313.1,1331.9,1334.2,1341.3,1350.6,1349.8,1346.4,1343.4,1344.9,1335.6,1337.9,1342.5,1337,1338.6,1337,1340.4,1324.65,1324.35,1349.5,1371.3,1367.9,1351.3,1357.8,1356.1,1356,1347.6,1339.1,1320.6,1311.8,1314,1312.4,1312.3,1323.5,1319.1,1327.2,1332.1,1320.3,1323.1,1328,1330.9,1338,1333,1335.3,1345.2,1341.1,1332.5,1314,1314.4,1310.7,1314,1313.1,1315,1313.7,1320,1326.5,1329.2,1314.2,1312.3,1309.5,1297.4,1293.7,1277.9,1295.8,1295.2,1290.3,1294.2,1298,1306.4,1299.8,1302.3,1297,1289.6,1302,1300.7,1303.5,1300.5,1303.2,1306,1318.7,1315,1314.5,1304.1,1294.7,1293.7,1291.2,1290.2,1300.4,1284.2,1284.25,1301.8,1295.9,1296.2,1304.4,1323.1,1340.9,1341,1348,1351.4,1351.4,1343.5,1342.3,1349,1357.6,1357.1,1354.7,1361.4,1375.2,1403.5,1414.7,1433.2,1438,1423.6,1424.4,1418,1399.5,1435.5,1421.25,1434.1,1412.4,1409.8,1412.2,1433.4,1418.4,1429,1428.8,1420.6,1441,1460.4,1441.7,1438.4,1431,1439.3,1427.4,1431.9,1439.5,1443.7,1425.6,1457.5,1451.2,1481.1,1486.7,1512.1,1515.9,1509.2,1522.3,1513,1526.6,1533.9,1523,1506.3,1518.4,1512.4,1508.8,1545.4,1537.3,1551.8,1549.4,1536.9,1535.25,1537.95,1535.2,1556,1561.4,1525.6,1516.4,1507,1493.9,1504.9,1506.5,1513.1,1506.5,1509.7,1502,1506.8,1521.5,1529.8,1539.8,1510.9,1511.8,1501.7,1478,1485.4,1505.6,1511.6,1518.6,1498.7,1510.9,1510.8,1498.3,1492,1497.7,1484.8,1494.2,1495.6,1495.6,1487.5,1491.1,1495.1,1506.4};
    double highs[]  = {1284.75,1320.6,1327,1330.8,1326.8,1321.6,1326,1328,1325.8,1327.1,1326,1326,1323.5,1322.1,1282.7,1282.95,1315.8,1316.3,1314,1333.2,1334.7,1341.7,1353.2,1354.6,1352.2,1346.4,1345.7,1344.9,1340.7,1344.2,1342.7,1342.1,1345.2,1342,1350,1324.95,1330.75,1369.6,1374.3,1368.4,1359.8,1359,1357,1356,1353.4,1340.6,1322.3,1314.1,1316.1,1312.9,1325.7,1323.5,1326.3,1336,1332.1,1330.1,1330.4,1334.7,1341.1,1344.2,1338.8,1348.4,1345.6,1342.8,1334.7,1322.3,1319.3,1314.7,1316.6,1316.4,1315,1325.4,1328.3,1332.2,1329.2,1316.9,1312.3,1309.5,1299.6,1296.9,1277.9,1299.5,1296.2,1298.4,1302.5,1308.7,1306.4,1305.9,1307,1297.2,1301.7,1305,1305.3,1310.2,1307,1308,1319.8,1321.7,1318.7,1316.2,1305.9,1295.8,1293.8,1293.7,1304.2,1302,1285.15,1286.85,1304,1302,1305.2,1323,1344.1,1345.2,1360.1,1355.3,1363.8,1353,1344.7,1353.6,1358,1373.6,1358.2,1369.6,1377.6,1408.9,1425.5,1435.9,1453.7,1438,1426,1439.1,1418,1435,1452.6,1426.65,1437.5,1421.5,1414.1,1433.3,1441.3,1431.4,1433.9,1432.4,1440.8,1462.3,1467,1443.5,1444,1442.9,1447,1437.6,1440.8,1445.7,1447.8,1458.2,1461.9,1481.8,1486.8,1522.7,1521.3,1521.1,1531.5,1546.1,1534.9,1537.7,1538.6,1523.6,1518.8,1518.4,1514.6,1540.3,1565,1554.5,1556.6,1559.8,1541.9,1542.9,1540.05,1558.9,1566.2,1561.9,1536.2,1523.8,1509.1,1506.2,1532.2,1516.6,1519.7,1515,1519.5,1512.1,1524.5,1534.4,1543.3,1543.3,1542.8,1519.5,1507.2,1493.5,1511.4,1525.8,1522.2,1518.8,1515.3,1518,1522.3,1508,1501.5,1503,1495.5,1501.1,1497.9,1498.7,1492.1,1499.4,1506.9,1520.9};
    double lows[]   = {1282.85,1315,1318.7,1309.6,1317.6,1312.9,1312.4,1319.1,1319,1321,1318.1,1321.3,1319.9,1312,1280.5,1276.15,1308,1309.9,1308.5,1312.3,1329.3,1333.1,1340.2,1347,1345.9,1338,1340.8,1335,1332,1337.9,1333,1336.8,1333.2,1329.9,1340.4,1323.85,1324.05,1349,1366.3,1351.2,1349.1,1352.4,1350.7,1344.3,1338.9,1316.3,1308.4,1306.9,1309.6,1306.7,1312.3,1315.4,1319,1327.2,1317.2,1320,1323,1328,1323,1327.8,1331.7,1335.3,1336.6,1331.8,1311.4,1310,1309.5,1308,1310.6,1302.8,1306.6,1313.7,1320,1322.8,1311,1312.1,1303.6,1293.9,1293.5,1291,1277.9,1294.1,1286,1289.1,1293.5,1296.9,1298,1299.6,1292.9,1285.1,1288.5,1296.3,1297.2,1298.4,1298.6,1302,1300.3,1312,1310.8,1301.9,1292,1291.1,1286.3,1289.2,1289.9,1297.4,1283.65,1283.25,1292.9,1295.9,1290.8,1304.2,1322.7,1336.1,1341,1343.5,1345.8,1340.3,1335.1,1341.5,1347.6,1352.8,1348.2,1353.7,1356.5,1373.3,1398,1414.7,1427,1416.4,1412.7,1420.1,1396.4,1398.8,1426.6,1412.85,1400.7,1406,1399.8,1404.4,1415.5,1417.2,1421.9,1415,1413.7,1428.1,1434,1435.7,1427.5,1429.4,1423.9,1425.6,1427.5,1434.8,1422.3,1412.1,1442.5,1448.8,1468.2,1484.3,1501.6,1506.2,1498.6,1488.9,1504.5,1518.3,1513.9,1503.3,1503,1506.5,1502.1,1503,1534.8,1535.3,1541.4,1528.6,1525.6,1535.25,1528.15,1528,1542.6,1514.3,1510.7,1505.5,1492.1,1492.9,1496.8,1493.1,1503.4,1500.9,1490.7,1496.3,1505.3,1505.3,1517.9,1507.4,1507.1,1493.3,1470.5,1465,1480.5,1501.7,1501.4,1493.3,1492.1,1505.1,1495.7,1478,1487.1,1480.8,1480.6,1487,1488.3,1484.8,1484,1490.7,1490.4,1503.1};
    double closes[] = {1283.35,1315.3,1326.1,1317.4,1321.5,1317.4,1323.5,1319.2,1321.3,1323.3,1319.7,1325.1,1323.6,1313.8,1282.05,1279.05,1314.2,1315.2,1310.8,1329.1,1334.5,1340.2,1340.5,1350,1347.1,1344.3,1344.6,1339.7,1339.4,1343.7,1337,1338.9,1340.1,1338.7,1346.8,1324.25,1329.55,1369.6,1372.5,1352.4,1357.6,1354.2,1353.4,1346,1341,1323.8,1311.9,1309.1,1312.2,1310.7,1324.3,1315.7,1322.4,1333.8,1319.4,1327.1,1325.8,1330.9,1325.8,1331.6,1336.5,1346.7,1339.2,1334.7,1313.3,1316.5,1312.4,1313.4,1313.3,1312.2,1313.7,1319.9,1326.3,1331.9,1311.3,1313.4,1309.4,1295.2,1294.7,1294.1,1277.9,1295.8,1291.2,1297.4,1297.7,1306.8,1299.4,1303.6,1302.2,1289.9,1299.2,1301.8,1303.6,1299.5,1303.2,1305.3,1319.5,1313.6,1315.1,1303.5,1293,1294.6,1290.4,1291.4,1302.7,1301,1284.15,1284.95,1294.3,1297.9,1304.1,1322.6,1339.3,1340.1,1344.9,1354,1357.4,1340.7,1342.7,1348.2,1355.1,1355.9,1354.2,1362.1,1360.1,1408.3,1411.2,1429.5,1430.1,1426.8,1423.4,1425.1,1400.8,1419.8,1432.9,1423.55,1412.1,1412.2,1412.8,1424.9,1419.3,1424.8,1426.1,1423.6,1435.9,1440.8,1439.4,1439.7,1434.5,1436.5,1427.5,1432.2,1433.3,1441.8,1437.8,1432.4,1457.5,1476.5,1484.2,1519.6,1509.5,1508.5,1517.2,1514.1,1527.8,1531.2,1523.6,1511.6,1515.7,1515.7,1508.5,1537.6,1537.2,1551.8,1549.1,1536.9,1529.4,1538.05,1535.15,1555.9,1560.4,1525.5,1515.5,1511.1,1499.2,1503.2,1507.4,1499.5,1511.5,1513.4,1515.8,1506.2,1515.1,1531.5,1540.2,1512.3,1515.2,1506.4,1472.9,1489,1507.9,1513.8,1512.9,1504.4,1503.9,1512.8,1500.9,1488.7,1497.6,1483.5,1494,1498.3,1494.1,1488.1,1487.5,1495.7,1504.7,1505.3};
    static bool tooltip = true;
    ImGui::Checkbox("Show Tooltip", &tooltip);
    ImGui::SameLine();
    static ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
    static ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);
    ImGui::SameLine(); ImGui::ColorEdit4("##Bull", &bullCol.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine(); ImGui::ColorEdit4("##Bear", &bearCol.x, ImGuiColorEditFlags_NoInputs);
    ImPlot::GetStyle().UseLocalTime = false;

    if (ImPlot::BeginPlot("Candlestick Chart",ImVec2(-1,0))) {
        ImPlot::SetupAxes(nullptr,nullptr,0,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit);
        ImPlot::SetupAxesLimits(1546300800, 1571961600, 1250, 1600);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 1546300800, 1571961600);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60*60*24*14, 1571961600-1546300800);
        ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
        MyImPlot::PlotCandlestick("GOOGL",dates, opens, closes, lows, highs, 218, tooltip, 0.25f, bullCol, bearCol);
        ImPlot::EndPlot();
    }
    }

//-----------------------------------------------------------------------------
// DEMO WINDOW
//-----------------------------------------------------------------------------

void DemoHeader(const char* label, void(*demo)()) {
    if (ImGui::TreeNodeEx(label)) {
        demo();
        ImGui::TreePop();
    }
}

void ShowDemoWindow(bool* p_open) {
    static bool show_implot_metrics      = false;
    static bool show_implot_style_editor = false;
    static bool show_imgui_metrics       = false;
    static bool show_imgui_style_editor  = false;
    static bool show_imgui_demo          = false;

    if (show_implot_metrics) {
        ImPlot::ShowMetricsWindow(&show_implot_metrics);
    }
    if (show_implot_style_editor) {
        ImGui::SetNextWindowSize(ImVec2(415,762), ImGuiCond_Appearing);
        ImGui::Begin("Style Editor (ImPlot)", &show_implot_style_editor);
        ImPlot::ShowStyleEditor();
        ImGui::End();
    }
    if (show_imgui_style_editor) {
        ImGui::Begin("Style Editor (ImGui)", &show_imgui_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (show_imgui_metrics) {
        ImGui::ShowMetricsWindow(&show_imgui_metrics);
    }
    if (show_imgui_demo) {
        ImGui::ShowDemoWindow(&show_imgui_demo);
    }
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("ImPlot Demo", p_open, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Metrics",      nullptr, &show_implot_metrics);
            ImGui::MenuItem("Style Editor", nullptr, &show_implot_style_editor);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Metrics",       nullptr, &show_imgui_metrics);
            ImGui::MenuItem("ImGui Style Editor",  nullptr, &show_imgui_style_editor);
            ImGui::MenuItem("ImGui Demo",          nullptr, &show_imgui_demo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    //-------------------------------------------------------------------------
    ImGui::Text("ImPlot says hello. (%s)", IMPLOT_VERSION);
    // display warning about 16-bit indices
    static bool showWarning = sizeof(ImDrawIdx)*8 == 16 && (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) == false;
    if (showWarning) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,0,1));
        ImGui::TextWrapped("WARNING: ImDrawIdx is 16-bit and ImGuiBackendFlags_RendererHasVtxOffset is false. Expect visual glitches and artifacts! See README for more information.");
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();

    if (ImGui::BeginTabBar("ImPlotDemoTabs")) {
        if (ImGui::BeginTabItem("Plots")) {
            DemoHeader("Line Plots", Demo_LinePlots);
            DemoHeader("Filled Line Plots", Demo_FilledLinePlots);
            DemoHeader("Shaded Plots##", Demo_ShadedPlots);
            DemoHeader("Scatter Plots", Demo_ScatterPlots);
            DemoHeader("Realtime Plots", Demo_RealtimePlots);
            DemoHeader("Stairstep Plots", Demo_StairstepPlots);
            DemoHeader("Bar Plots", Demo_BarPlots);
            DemoHeader("Bar Groups", Demo_BarGroups);
            DemoHeader("Bar Stacks", Demo_BarStacks);
            DemoHeader("Error Bars", Demo_ErrorBars);
            DemoHeader("Stem Plots##", Demo_StemPlots);
            DemoHeader("Infinite Lines", Demo_InfiniteLines);
            DemoHeader("Pie Charts", Demo_PieCharts);
            DemoHeader("Heatmaps", Demo_Heatmaps);
            DemoHeader("Histogram", Demo_Histogram);
            DemoHeader("Histogram 2D", Demo_Histogram2D);
            DemoHeader("Digital Plots", Demo_DigitalPlots);
            DemoHeader("Images", Demo_Images);
            DemoHeader("Markers and Text", Demo_MarkersAndText);
            DemoHeader("NaN Values", Demo_NaNValues);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Subplots")) {
            DemoHeader("Sizing", Demo_SubplotsSizing);
            DemoHeader("Item Sharing", Demo_SubplotItemSharing);
            DemoHeader("Axis Linking", Demo_SubplotAxisLinking);
            DemoHeader("Tables", Demo_Tables);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Axes")) {
            DemoHeader("Log Scale", Demo_LogScale);
            DemoHeader("Symmetric Log Scale", Demo_SymmetricLogScale);
            DemoHeader("Time Scale", Demo_TimeScale);
            DemoHeader("Custom Scale", Demo_CustomScale);
            DemoHeader("Multiple Axes", Demo_MultipleAxes);
            DemoHeader("Tick Labels", Demo_TickLabels);
            DemoHeader("Linked Axes", Demo_LinkedAxes);
            DemoHeader("Axis Constraints", Demo_AxisConstraints);
            DemoHeader("Equal Axes", Demo_EqualAxes);
            DemoHeader("Auto-Fitting Data", Demo_AutoFittingData);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tools")) {
            DemoHeader("Offset and Stride", Demo_OffsetAndStride);
            DemoHeader("Drag Points", Demo_DragPoints);
            DemoHeader("Drag Lines", Demo_DragLines);
            DemoHeader("Drag Rects", Demo_DragRects);
            DemoHeader("Querying", Demo_Querying);
            DemoHeader("Annotations", Demo_Annotations);
            DemoHeader("Tags", Demo_Tags);
            DemoHeader("Drag and Drop", Demo_DragAndDrop);
            DemoHeader("Legend Options", Demo_LegendOptions);
            DemoHeader("Legend Popups", Demo_LegendPopups);
            DemoHeader("Colormap Widgets", Demo_ColormapWidgets);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Custom")) {
            DemoHeader("Custom Styles", Demo_CustomStyles);
            DemoHeader("Custom Data and Getters", Demo_CustomDataAndGetters);
            DemoHeader("Custom Rendering", Demo_CustomRendering);
            DemoHeader("Custom Plotters and Tooltips", Demo_CustomPlottersAndTooltips);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            Demo_Config();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Help")) {
            Demo_Help();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

} // namespace ImPlot

namespace MyImPlot {

ImPlotPoint SineWave(int idx, void* data) {
    WaveData* wd = (WaveData*)data;
    double x = idx * wd->X;
    return ImPlotPoint(x, wd->Offset + wd->Amp * sin(2 * 3.14 * wd->Freq * x));
}

ImPlotPoint SawWave(int idx, void* data) {
    WaveData* wd = (WaveData*)data;
    double x = idx * wd->X;
    return ImPlotPoint(x, wd->Offset + wd->Amp * (-2 / 3.14 * atan(cos(3.14 * wd->Freq * x) / sin(3.14 * wd->Freq * x))));
}

ImPlotPoint Spiral(int idx, void*) {
    float r = 0.9f;            // outer radius
    float a = 0;               // inner radius
    float b = 0.05f;           // increment per rev
    float n = (r - a) / b;     // number  of revolutions
    double th = 2 * n * 3.14;  // angle
    float Th = float(th * idx / (1000 - 1));
    return ImPlotPoint(0.5f+(a + b*Th / (2.0f * (float) 3.14))*cos(Th),
                       0.5f + (a + b*Th / (2.0f * (float)3.14))*sin(Th));
}

void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size) {
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
    if (ImPlot::BeginPlot(id,size,ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
        ImPlot::SetNextLineStyle(col);
        ImPlot::SetNextFillStyle(col, 0.25);
        ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
}

void StyleSeaborn() {

    ImPlotStyle& style              = ImPlot::GetStyle();

    ImVec4* colors                  = style.Colors;
    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.80f, 0.81f, 0.85f, 1.00f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisText]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisGrid]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_AxisBgHovered]   = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_AxisBgActive]    = ImVec4(0.92f, 0.92f, 0.95f, 0.75f);
    colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.65f, 0.00f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.23f, 0.10f, 0.64f, 0.50f);

    style.LineWeight       = 1.5;
    style.Marker           = ImPlotMarker_None;
    style.MarkerSize       = 4;
    style.MarkerWeight     = 1;
    style.FillAlpha        = 1.0f;
    style.ErrorBarSize     = 5;
    style.ErrorBarWeight   = 1.5f;
    style.DigitalBitHeight = 8;
    style.DigitalBitGap    = 4;
    style.PlotBorderSize   = 0;
    style.MinorAlpha       = 1.0f;
    style.MajorTickLen     = ImVec2(0,0);
    style.MinorTickLen     = ImVec2(0,0);
    style.MajorTickSize    = ImVec2(0,0);
    style.MinorTickSize    = ImVec2(0,0);
    style.MajorGridSize    = ImVec2(1.2f,1.2f);
    style.MinorGridSize    = ImVec2(1.2f,1.2f);
    style.PlotPadding      = ImVec2(12,12);
    style.LabelPadding     = ImVec2(5,5);
    style.LegendPadding    = ImVec2(5,5);
    style.MousePosPadding  = ImVec2(5,5);
    style.PlotMinSize      = ImVec2(300,225);
}

} // namespaece MyImPlot

// WARNING:
//
// You can use "implot_internal.h" to build custom plotting fuctions or extend ImPlot.
// However, note that forward compatibility of this file is not guaranteed and the
// internal API is subject to change. At some point we hope to bring more of this
// into the public API and expose the necessary building blocks to fully support
// custom plotters. For now, proceed at your own risk!

//#include "implot_internal.h"

namespace MyImPlot {

template <typename T>
int BinarySearch(const T* arr, int l, int r, T x) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x)
            return mid;
        if (arr[mid] > x)
            return BinarySearch(arr, l, mid - 1, x);
        return BinarySearch(arr, mid + 1, r, x);
    }
    return -1;
}

void PlotCandlestick(const char* label_id, const double* xs, const double* opens, const double* closes, const double* lows, const double* highs, int count, bool tooltip, float width_percent, ImVec4 bullCol, ImVec4 bearCol) {

    // get ImGui window DrawList
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    // calc real value width
    double half_width = count > 1 ? (xs[1] - xs[0]) * width_percent : width_percent;

    // custom tool
    if (ImPlot::IsPlotHovered() && tooltip) {
        ImPlotPoint mouse   = ImPlot::GetPlotMousePos();
        mouse.x             = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day).ToDouble();
        float  tool_l       = ImPlot::PlotToPixels(mouse.x - half_width * 1.5, mouse.y).x;
        float  tool_r       = ImPlot::PlotToPixels(mouse.x + half_width * 1.5, mouse.y).x;
        float  tool_t       = ImPlot::GetPlotPos().y;
        float  tool_b       = tool_t + ImPlot::GetPlotSize().y;
        ImPlot::PushPlotClipRect();
        draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b), IM_COL32(128,128,128,64));
        ImPlot::PopPlotClipRect();
        // find mouse location index
        int idx = BinarySearch(xs, 0, count - 1, mouse.x);
        // render tool tip (won't be affected by plot clip rect)
        if (idx != -1) {
            ImGui::BeginTooltip();
            char buff[32];
            ImPlot::FormatDate(ImPlotTime::FromDouble(xs[idx]),buff,32,ImPlotDateFmt_DayMoYr,ImPlot::GetStyle().UseISO8601);
            ImGui::Text("Day:   %s",  buff);
            ImGui::Text("Open:  $%.2f", opens[idx]);
            ImGui::Text("Close: $%.2f", closes[idx]);
            ImGui::Text("Low:   $%.2f", lows[idx]);
            ImGui::Text("High:  $%.2f", highs[idx]);
            ImGui::EndTooltip();
        }
    }

    // begin plot item
    if (ImPlot::BeginItem(label_id)) {
        // override legend icon color
        ImPlot::GetCurrentItem()->Color = IM_COL32(64,64,64,255);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < count; ++i) {
                ImPlot::FitPoint(ImPlotPoint(xs[i], lows[i]));
                ImPlot::FitPoint(ImPlotPoint(xs[i], highs[i]));
            }
        }
        // render data
        for (int i = 0; i < count; ++i) {
            ImVec2 open_pos  = ImPlot::PlotToPixels(xs[i] - half_width, opens[i]);
            ImVec2 close_pos = ImPlot::PlotToPixels(xs[i] + half_width, closes[i]);
            ImVec2 low_pos   = ImPlot::PlotToPixels(xs[i], lows[i]);
            ImVec2 high_pos  = ImPlot::PlotToPixels(xs[i], highs[i]);
            ImU32 color      = ImGui::GetColorU32(opens[i] > closes[i] ? bearCol : bullCol);
            draw_list->AddLine(low_pos, high_pos, color);
            draw_list->AddRectFilled(open_pos, close_pos, color);
        }

        // end plot item
        ImPlot::EndItem();
    }
}

} // namespace MyImplot

// MIT License

// Copyright (c) 2023 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.17

//#define IMGUI_DEFINE_MATH_OPERATORS
//#include "implot.h"
//#include "implot_internal.h"

//-----------------------------------------------------------------------------
// [SECTION] Macros and Defines
//-----------------------------------------------------------------------------

#define SQRT_1_2 0.70710678118f
#define SQRT_3_2 0.86602540378f

#ifndef IMPLOT_NO_FORCE_INLINE
    #ifdef _MSC_VER
        #define IMPLOT_INLINE __forceinline
    #elif defined(__GNUC__)
        #define IMPLOT_INLINE inline __attribute__((__always_inline__))
    #elif defined(__CLANG__)
        #if __has_attribute(__always_inline__)
            #define IMPLOT_INLINE inline __attribute__((__always_inline__))
        #else
            #define IMPLOT_INLINE inline
        #endif
    #else
        #define IMPLOT_INLINE inline
    #endif
#else
    #define IMPLOT_INLINE inline
#endif

#if defined __SSE__ || defined __x86_64__ || defined _M_X64
#ifndef IMGUI_ENABLE_SSE
#include <immintrin.h>
#endif
static IMPLOT_INLINE float  ImInvSqrt(float x) { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static IMPLOT_INLINE float  ImInvSqrt(float x) { return 1.0f / sqrtf(x); }
#endif

#define IMPLOT_NORMALIZE2F_OVER_ZERO(VX,VY) do { float d2 = VX*VX + VY*VY; if (d2 > 0.0f) { float inv_len = ImInvSqrt(d2); VX *= inv_len; VY *= inv_len; } } while (0)

// Support for pre-1.82 versions. Users on 1.82+ can use 0 (default) flags to mean "all corners" but in order to support older versions we are more explicit.
#if (IMGUI_VERSION_NUM < 18102) && !defined(ImDrawFlags_RoundCornersAll)
#define ImDrawFlags_RoundCornersAll ImDrawCornerFlags_All
#endif

//-----------------------------------------------------------------------------
// [SECTION] Template instantiation utility
//-----------------------------------------------------------------------------

// By default, templates are instantiated for `float`, `double`, and for the following integer types, which are defined in imgui.h:
//     signed char         ImS8;   // 8-bit signed integer
//     unsigned char       ImU8;   // 8-bit unsigned integer
//     signed short        ImS16;  // 16-bit signed integer
//     unsigned short      ImU16;  // 16-bit unsigned integer
//     signed int          ImS32;  // 32-bit signed integer == int
//     unsigned int        ImU32;  // 32-bit unsigned integer
//     signed   long long  ImS64;  // 64-bit signed integer
//     unsigned long long  ImU64;  // 64-bit unsigned integer
// (note: this list does *not* include `long`, `unsigned long` and `long double`)
//
// You can customize the supported types by defining IMPLOT_CUSTOM_NUMERIC_TYPES at compile time to define your own type list.
//    As an example, you could use the compile time define given by the line below in order to support only float and double.
//        -DIMPLOT_CUSTOM_NUMERIC_TYPES="(float)(double)"
//    In order to support all known C++ types, use:
//        -DIMPLOT_CUSTOM_NUMERIC_TYPES="(signed char)(unsigned char)(signed short)(unsigned short)(signed int)(unsigned int)(signed long)(unsigned long)(signed long long)(unsigned long long)(float)(double)(long double)"

#ifdef IMPLOT_CUSTOM_NUMERIC_TYPES
    #define IMPLOT_NUMERIC_TYPES IMPLOT_CUSTOM_NUMERIC_TYPES
#else
    #define IMPLOT_NUMERIC_TYPES (ImS8)(ImU8)(ImS16)(ImU16)(ImS32)(ImU32)(ImS64)(ImU64)(float)(double)
#endif

// CALL_INSTANTIATE_FOR_NUMERIC_TYPES will duplicate the template instantion code `INSTANTIATE_MACRO(T)` on supported types.
#define _CAT(x, y) _CAT_(x, y)
#define _CAT_(x,y) x ## y
#define _INSTANTIATE_FOR_NUMERIC_TYPES(chain) _CAT(_INSTANTIATE_FOR_NUMERIC_TYPES_1 chain, _END)
#define _INSTANTIATE_FOR_NUMERIC_TYPES_1(T) INSTANTIATE_MACRO(T) _INSTANTIATE_FOR_NUMERIC_TYPES_2
#define _INSTANTIATE_FOR_NUMERIC_TYPES_2(T) INSTANTIATE_MACRO(T) _INSTANTIATE_FOR_NUMERIC_TYPES_1
#define _INSTANTIATE_FOR_NUMERIC_TYPES_1_END
#define _INSTANTIATE_FOR_NUMERIC_TYPES_2_END
#define CALL_INSTANTIATE_FOR_NUMERIC_TYPES() _INSTANTIATE_FOR_NUMERIC_TYPES(IMPLOT_NUMERIC_TYPES)

namespace ImPlot {

//-----------------------------------------------------------------------------
// [SECTION] Utils
//-----------------------------------------------------------------------------

// Calc maximum index size of ImDrawIdx
template <typename T>
struct MaxIdx { static const unsigned int Value; };
template <> const unsigned int MaxIdx<unsigned short>::Value = 65535;
template <> const unsigned int MaxIdx<unsigned int>::Value   = 4294967295;

IMPLOT_INLINE void GetLineRenderProps(const ImDrawList& draw_list, float& half_weight, ImVec2& tex_uv0, ImVec2& tex_uv1) {
    const bool aa = ImHasFlag(draw_list.Flags, ImDrawListFlags_AntiAliasedLines) &&
                    ImHasFlag(draw_list.Flags, ImDrawListFlags_AntiAliasedLinesUseTex);
    if (aa) {
        ImVec4 tex_uvs = draw_list._Data->TexUvLines[(int)(half_weight*2)];
        tex_uv0 = ImVec2(tex_uvs.x, tex_uvs.y);
        tex_uv1 = ImVec2(tex_uvs.z, tex_uvs.w);
        half_weight += 1;
    }
    else {
        tex_uv0 = tex_uv1 = draw_list._Data->TexUvWhitePixel;
    }
}

IMPLOT_INLINE void PrimLine(ImDrawList& draw_list, const ImVec2& P1, const ImVec2& P2, float half_weight, ImU32 col, const ImVec2& tex_uv0, const ImVec2 tex_uv1) {
    float dx = P2.x - P1.x;
    float dy = P2.y - P1.y;
    IMPLOT_NORMALIZE2F_OVER_ZERO(dx, dy);
    dx *= half_weight;
    dy *= half_weight;
    draw_list._VtxWritePtr[0].pos.x = P1.x + dy;
    draw_list._VtxWritePtr[0].pos.y = P1.y - dx;
    draw_list._VtxWritePtr[0].uv    = tex_uv0;
    draw_list._VtxWritePtr[0].col   = col;
    draw_list._VtxWritePtr[1].pos.x = P2.x + dy;
    draw_list._VtxWritePtr[1].pos.y = P2.y - dx;
    draw_list._VtxWritePtr[1].uv    = tex_uv0;
    draw_list._VtxWritePtr[1].col   = col;
    draw_list._VtxWritePtr[2].pos.x = P2.x - dy;
    draw_list._VtxWritePtr[2].pos.y = P2.y + dx;
    draw_list._VtxWritePtr[2].uv    = tex_uv1;
    draw_list._VtxWritePtr[2].col   = col;
    draw_list._VtxWritePtr[3].pos.x = P1.x - dy;
    draw_list._VtxWritePtr[3].pos.y = P1.y + dx;
    draw_list._VtxWritePtr[3].uv    = tex_uv1;
    draw_list._VtxWritePtr[3].col   = col;
    draw_list._VtxWritePtr += 4;
    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[3] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
    draw_list._IdxWritePtr[4] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[5] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
    draw_list._IdxWritePtr += 6;
    draw_list._VtxCurrentIdx += 4;
}

IMPLOT_INLINE void PrimRectFill(ImDrawList& draw_list, const ImVec2& Pmin, const ImVec2& Pmax, ImU32 col, const ImVec2& uv) {
    draw_list._VtxWritePtr[0].pos   = Pmin;
    draw_list._VtxWritePtr[0].uv    = uv;
    draw_list._VtxWritePtr[0].col   = col;
    draw_list._VtxWritePtr[1].pos   = Pmax;
    draw_list._VtxWritePtr[1].uv    = uv;
    draw_list._VtxWritePtr[1].col   = col;
    draw_list._VtxWritePtr[2].pos.x = Pmin.x;
    draw_list._VtxWritePtr[2].pos.y = Pmax.y;
    draw_list._VtxWritePtr[2].uv    = uv;
    draw_list._VtxWritePtr[2].col   = col;
    draw_list._VtxWritePtr[3].pos.x = Pmax.x;
    draw_list._VtxWritePtr[3].pos.y = Pmin.y;
    draw_list._VtxWritePtr[3].uv    = uv;
    draw_list._VtxWritePtr[3].col   = col;
    draw_list._VtxWritePtr += 4;
    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[3] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
    draw_list._IdxWritePtr[4] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[5] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
    draw_list._IdxWritePtr += 6;
    draw_list._VtxCurrentIdx += 4;
}

IMPLOT_INLINE void PrimRectLine(ImDrawList& draw_list, const ImVec2& Pmin, const ImVec2& Pmax, float weight, ImU32 col, const ImVec2& uv) {

    draw_list._VtxWritePtr[0].pos.x = Pmin.x;
    draw_list._VtxWritePtr[0].pos.y = Pmin.y;
    draw_list._VtxWritePtr[0].uv    = uv;
    draw_list._VtxWritePtr[0].col   = col;

    draw_list._VtxWritePtr[1].pos.x = Pmin.x;
    draw_list._VtxWritePtr[1].pos.y = Pmax.y;
    draw_list._VtxWritePtr[1].uv    = uv;
    draw_list._VtxWritePtr[1].col   = col;

    draw_list._VtxWritePtr[2].pos.x = Pmax.x;
    draw_list._VtxWritePtr[2].pos.y = Pmax.y;
    draw_list._VtxWritePtr[2].uv    = uv;
    draw_list._VtxWritePtr[2].col   = col;

    draw_list._VtxWritePtr[3].pos.x = Pmax.x;
    draw_list._VtxWritePtr[3].pos.y = Pmin.y;
    draw_list._VtxWritePtr[3].uv    = uv;
    draw_list._VtxWritePtr[3].col   = col;

    draw_list._VtxWritePtr[4].pos.x = Pmin.x + weight;
    draw_list._VtxWritePtr[4].pos.y = Pmin.y + weight;
    draw_list._VtxWritePtr[4].uv    = uv;
    draw_list._VtxWritePtr[4].col   = col;

    draw_list._VtxWritePtr[5].pos.x = Pmin.x + weight;
    draw_list._VtxWritePtr[5].pos.y = Pmax.y - weight;
    draw_list._VtxWritePtr[5].uv    = uv;
    draw_list._VtxWritePtr[5].col   = col;

    draw_list._VtxWritePtr[6].pos.x = Pmax.x - weight;
    draw_list._VtxWritePtr[6].pos.y = Pmax.y - weight;
    draw_list._VtxWritePtr[6].uv    = uv;
    draw_list._VtxWritePtr[6].col   = col;

    draw_list._VtxWritePtr[7].pos.x = Pmax.x - weight;
    draw_list._VtxWritePtr[7].pos.y = Pmin.y + weight;
    draw_list._VtxWritePtr[7].uv    = uv;
    draw_list._VtxWritePtr[7].col   = col;

    draw_list._VtxWritePtr += 8;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 0);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 5);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 0);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 5);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 4);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 6);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 6);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 5);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 7);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 7);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 6);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 0);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 4);
    draw_list._IdxWritePtr += 3;

    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 4);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 7);
    draw_list._IdxWritePtr += 3;

    draw_list._VtxCurrentIdx += 8;
}


//-----------------------------------------------------------------------------
// [SECTION] Item Utils
//-----------------------------------------------------------------------------

ImPlotItem* RegisterOrGetItem(const char* label_id, ImPlotItemFlags flags, bool* just_created) {
    ImPlotContext& gp = *GImPlot;
    ImPlotItemGroup& Items = *gp.CurrentItems;
    ImGuiID id = Items.GetItemID(label_id);
    if (just_created != nullptr)
        *just_created = Items.GetItem(id) == nullptr;
    ImPlotItem* item = Items.GetOrAddItem(id);
    if (item->SeenThisFrame)
        return item;
    item->SeenThisFrame = true;
    int idx = Items.GetItemIndex(item);
    item->ID = id;
    if (!ImHasFlag(flags, ImPlotItemFlags_NoLegend) && ImGui::FindRenderedTextEnd(label_id, nullptr) != label_id) {
        Items.Legend.Indices.push_back(idx);
        item->NameOffset = Items.Legend.Labels.size();
        Items.Legend.Labels.append(label_id, label_id + strlen(label_id) + 1);
    }
    else {
        item->Show = true;
    }
    return item;
}

ImPlotItem* GetItem(const char* label_id) {
    ImPlotContext& gp = *GImPlot;
    return gp.CurrentItems->GetItem(label_id);
}

bool IsItemHidden(const char* label_id) {
    ImPlotItem* item = GetItem(label_id);
    return item != nullptr && !item->Show;
}

ImPlotItem* GetCurrentItem() {
    ImPlotContext& gp = *GImPlot;
    return gp.CurrentItem;
}

void SetNextLineStyle(const ImVec4& col, float weight) {
    ImPlotContext& gp = *GImPlot;
    gp.NextItemData.Colors[ImPlotCol_Line] = col;
    gp.NextItemData.LineWeight             = weight;
}

void SetNextFillStyle(const ImVec4& col, float alpha) {
    ImPlotContext& gp = *GImPlot;
    gp.NextItemData.Colors[ImPlotCol_Fill] = col;
    gp.NextItemData.FillAlpha              = alpha;
}

void SetNextMarkerStyle(ImPlotMarker marker, float size, const ImVec4& fill, float weight, const ImVec4& outline) {
    ImPlotContext& gp = *GImPlot;
    gp.NextItemData.Marker                          = marker;
    gp.NextItemData.Colors[ImPlotCol_MarkerFill]    = fill;
    gp.NextItemData.MarkerSize                      = size;
    gp.NextItemData.Colors[ImPlotCol_MarkerOutline] = outline;
    gp.NextItemData.MarkerWeight                    = weight;
}

void SetNextErrorBarStyle(const ImVec4& col, float size, float weight) {
    ImPlotContext& gp = *GImPlot;
    gp.NextItemData.Colors[ImPlotCol_ErrorBar] = col;
    gp.NextItemData.ErrorBarSize               = size;
    gp.NextItemData.ErrorBarWeight             = weight;
}

ImVec4 GetLastItemColor() {
    ImPlotContext& gp = *GImPlot;
    if (gp.PreviousItem)
        return ImGui::ColorConvertU32ToFloat4(gp.PreviousItem->Color);
    return ImVec4();
}

void BustItemCache() {
    ImPlotContext& gp = *GImPlot;
    for (int p = 0; p < gp.Plots.GetBufSize(); ++p) {
        ImPlotPlot& plot = *gp.Plots.GetByIndex(p);
        plot.Items.Reset();
    }
    for (int p = 0; p < gp.Subplots.GetBufSize(); ++p) {
        ImPlotSubplot& subplot = *gp.Subplots.GetByIndex(p);
        subplot.Items.Reset();
    }
}

void BustColorCache(const char* plot_title_id) {
    ImPlotContext& gp = *GImPlot;
    if (plot_title_id == nullptr) {
        BustItemCache();
    }
    else {
        ImGuiID id = ImGui::GetCurrentWindow()->GetID(plot_title_id);
        ImPlotPlot* plot = gp.Plots.GetByKey(id);
        if (plot != nullptr)
            plot->Items.Reset();
        else {
            ImPlotSubplot* subplot = gp.Subplots.GetByKey(id);
            if (subplot != nullptr)
                subplot->Items.Reset();
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] BeginItem / EndItem
//-----------------------------------------------------------------------------

static const float ITEM_HIGHLIGHT_LINE_SCALE = 2.0f;
static const float ITEM_HIGHLIGHT_MARK_SCALE = 1.25f;

// Begins a new item. Returns false if the item should not be plotted.
bool BeginItem(const char* label_id, ImPlotItemFlags flags, ImPlotCol recolor_from) {
    ImPlotContext& gp = *GImPlot;
    IM_ASSERT_USER_ERROR(gp.CurrentPlot != nullptr, "PlotX() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    bool just_created;
    ImPlotItem* item = RegisterOrGetItem(label_id, flags, &just_created);
    // set current item
    gp.CurrentItem = item;
    ImPlotNextItemData& s = gp.NextItemData;
    // set/override item color
    if (recolor_from != -1) {
        if (!IsColorAuto(s.Colors[recolor_from]))
            item->Color = ImGui::ColorConvertFloat4ToU32(s.Colors[recolor_from]);
        else if (!IsColorAuto(gp.Style.Colors[recolor_from]))
            item->Color = ImGui::ColorConvertFloat4ToU32(gp.Style.Colors[recolor_from]);
        else if (just_created)
            item->Color = NextColormapColorU32();
    }
    else if (just_created) {
        item->Color = NextColormapColorU32();
    }
    // hide/show item
    if (gp.NextItemData.HasHidden) {
        if (just_created || gp.NextItemData.HiddenCond == ImGuiCond_Always)
            item->Show = !gp.NextItemData.Hidden;
    }
    if (!item->Show) {
        // reset next item data
        gp.NextItemData.Reset();
        gp.PreviousItem = item;
        gp.CurrentItem  = nullptr;
        return false;
    }
    else {
        ImVec4 item_color = ImGui::ColorConvertU32ToFloat4(item->Color);
        // stage next item colors
        s.Colors[ImPlotCol_Line]           = IsColorAuto(s.Colors[ImPlotCol_Line])          ? ( IsColorAuto(ImPlotCol_Line)           ? item_color                 : gp.Style.Colors[ImPlotCol_Line]          ) : s.Colors[ImPlotCol_Line];
        s.Colors[ImPlotCol_Fill]           = IsColorAuto(s.Colors[ImPlotCol_Fill])          ? ( IsColorAuto(ImPlotCol_Fill)           ? item_color                 : gp.Style.Colors[ImPlotCol_Fill]          ) : s.Colors[ImPlotCol_Fill];
        s.Colors[ImPlotCol_MarkerOutline]  = IsColorAuto(s.Colors[ImPlotCol_MarkerOutline]) ? ( IsColorAuto(ImPlotCol_MarkerOutline)  ? s.Colors[ImPlotCol_Line]   : gp.Style.Colors[ImPlotCol_MarkerOutline] ) : s.Colors[ImPlotCol_MarkerOutline];
        s.Colors[ImPlotCol_MarkerFill]     = IsColorAuto(s.Colors[ImPlotCol_MarkerFill])    ? ( IsColorAuto(ImPlotCol_MarkerFill)     ? s.Colors[ImPlotCol_Line]   : gp.Style.Colors[ImPlotCol_MarkerFill]    ) : s.Colors[ImPlotCol_MarkerFill];
        s.Colors[ImPlotCol_ErrorBar]       = IsColorAuto(s.Colors[ImPlotCol_ErrorBar])      ? ( GetStyleColorVec4(ImPlotCol_ErrorBar)                                                                         ) : s.Colors[ImPlotCol_ErrorBar];
        // stage next item style vars
        s.LineWeight         = s.LineWeight       < 0 ? gp.Style.LineWeight       : s.LineWeight;
        s.Marker             = s.Marker           < 0 ? gp.Style.Marker           : s.Marker;
        s.MarkerSize         = s.MarkerSize       < 0 ? gp.Style.MarkerSize       : s.MarkerSize;
        s.MarkerWeight       = s.MarkerWeight     < 0 ? gp.Style.MarkerWeight     : s.MarkerWeight;
        s.FillAlpha          = s.FillAlpha        < 0 ? gp.Style.FillAlpha        : s.FillAlpha;
        s.ErrorBarSize       = s.ErrorBarSize     < 0 ? gp.Style.ErrorBarSize     : s.ErrorBarSize;
        s.ErrorBarWeight     = s.ErrorBarWeight   < 0 ? gp.Style.ErrorBarWeight   : s.ErrorBarWeight;
        s.DigitalBitHeight   = s.DigitalBitHeight < 0 ? gp.Style.DigitalBitHeight : s.DigitalBitHeight;
        s.DigitalBitGap      = s.DigitalBitGap    < 0 ? gp.Style.DigitalBitGap    : s.DigitalBitGap;
        // apply alpha modifier(s)
        s.Colors[ImPlotCol_Fill].w       *= s.FillAlpha;
        s.Colors[ImPlotCol_MarkerFill].w *= s.FillAlpha; // TODO: this should be separate, if it at all
        // apply highlight mods
        if (item->LegendHovered) {
            if (!ImHasFlag(gp.CurrentItems->Legend.Flags, ImPlotLegendFlags_NoHighlightItem)) {
                s.LineWeight   *= ITEM_HIGHLIGHT_LINE_SCALE;
                s.MarkerSize   *= ITEM_HIGHLIGHT_MARK_SCALE;
                s.MarkerWeight *= ITEM_HIGHLIGHT_LINE_SCALE;
                // TODO: how to highlight fills?
            }
            if (!ImHasFlag(gp.CurrentItems->Legend.Flags, ImPlotLegendFlags_NoHighlightAxis)) {
                if (gp.CurrentPlot->EnabledAxesX() > 1)
                    gp.CurrentPlot->Axes[gp.CurrentPlot->CurrentX].ColorHiLi = item->Color;
                if (gp.CurrentPlot->EnabledAxesY() > 1)
                    gp.CurrentPlot->Axes[gp.CurrentPlot->CurrentY].ColorHiLi = item->Color;
            }
        }
        // set render flags
        s.RenderLine       = s.Colors[ImPlotCol_Line].w          > 0 && s.LineWeight > 0;
        s.RenderFill       = s.Colors[ImPlotCol_Fill].w          > 0;
        s.RenderMarkerFill = s.Colors[ImPlotCol_MarkerFill].w    > 0;
        s.RenderMarkerLine = s.Colors[ImPlotCol_MarkerOutline].w > 0 && s.MarkerWeight > 0;
        // push rendering clip rect
        PushPlotClipRect();
        return true;
    }
}

// Ends an item (call only if BeginItem returns true)
void EndItem() {
    ImPlotContext& gp = *GImPlot;
    // pop rendering clip rect
    PopPlotClipRect();
    // reset next item data
    gp.NextItemData.Reset();
    // set current item
    gp.PreviousItem = gp.CurrentItem;
    gp.CurrentItem  = nullptr;
}

//-----------------------------------------------------------------------------
// [SECTION] Indexers
//-----------------------------------------------------------------------------

template <typename T>
IMPLOT_INLINE T IndexData(const T* data, int idx, int count, int offset, int stride) {
    const int s = ((offset == 0) << 0) | ((stride == sizeof(T)) << 1);
    switch (s) {
        case 3 : return data[idx];
        case 2 : return data[(offset + idx) % count];
        case 1 : return *(const T*)(const void*)((const unsigned char*)data + (size_t)((idx) ) * stride);
        case 0 : return *(const T*)(const void*)((const unsigned char*)data + (size_t)((offset + idx) % count) * stride);
        default: return T(0);
    }
}

template <typename T>
struct IndexerIdx {
    IndexerIdx(const T* data, int count, int offset = 0, int stride = sizeof(T)) :
        Data(data),
        Count(count),
        Offset(count ? ImPosMod(offset, count) : 0),
        Stride(stride)
    { }
    template <typename I> IMPLOT_INLINE double operator()(I idx) const {
        return (double)IndexData(Data, idx, Count, Offset, Stride);
    }
    const T* Data;
    int Count;
    int Offset;
    int Stride;
};

template <typename _Indexer1, typename _Indexer2>
struct IndexerAdd {
    IndexerAdd(const _Indexer1& indexer1, const _Indexer2& indexer2, double scale1 = 1, double scale2 = 1)
        : Indexer1(indexer1),
          Indexer2(indexer2),
          Scale1(scale1),
          Scale2(scale2),
          Count(ImMin(Indexer1.Count, Indexer2.Count))
    { }
    template <typename I> IMPLOT_INLINE double operator()(I idx) const {
        return Scale1 * Indexer1(idx) + Scale2 * Indexer2(idx);
    }
    const _Indexer1& Indexer1;
    const _Indexer2& Indexer2;
    double Scale1;
    double Scale2;
    int Count;
};

struct IndexerLin {
    IndexerLin(double m, double b) : M(m), B(b) { }
    template <typename I> IMPLOT_INLINE double operator()(I idx) const {
        return M * idx + B;
    }
    const double M;
    const double B;
};

struct IndexerConst {
    IndexerConst(double ref) : Ref(ref) { }
    template <typename I> IMPLOT_INLINE double operator()(I) const { return Ref; }
    const double Ref;
};

//-----------------------------------------------------------------------------
// [SECTION] Getters
//-----------------------------------------------------------------------------

template <typename _IndexerX, typename _IndexerY>
struct GetterXY {
    GetterXY(_IndexerX x, _IndexerY y, int count) : IndxerX(x), IndxerY(y), Count(count) { }
    template <typename I> IMPLOT_INLINE ImPlotPoint operator()(I idx) const {
        return ImPlotPoint(IndxerX(idx),IndxerY(idx));
    }
    const _IndexerX IndxerX;
    const _IndexerY IndxerY;
    const int Count;
};

/// Interprets a user's function pointer as ImPlotPoints
struct GetterFuncPtr {
    GetterFuncPtr(ImPlotGetter getter, void* data, int count) :
        Getter(getter),
        Data(data),
        Count(count)
    { }
    template <typename I> IMPLOT_INLINE ImPlotPoint operator()(I idx) const {
        return Getter(idx, Data);
    }
    ImPlotGetter Getter;
    void* const Data;
    const int Count;
};

template <typename _Getter>
struct GetterOverrideX {
    GetterOverrideX(_Getter getter, double x) : Getter(getter), X(x), Count(getter.Count) { }
    template <typename I> IMPLOT_INLINE ImPlotPoint operator()(I idx) const {
        ImPlotPoint p = Getter(idx);
        p.x = X;
        return p;
    }
    const _Getter Getter;
    const double X;
    const int Count;
};

template <typename _Getter>
struct GetterOverrideY {
    GetterOverrideY(_Getter getter, double y) : Getter(getter), Y(y), Count(getter.Count) { }
    template <typename I> IMPLOT_INLINE ImPlotPoint operator()(I idx) const {
        ImPlotPoint p = Getter(idx);
        p.y = Y;
        return p;
    }
    const _Getter Getter;
    const double Y;
    const int Count;
};

template <typename _Getter>
struct GetterLoop {
    GetterLoop(_Getter getter) : Getter(getter), Count(getter.Count + 1) { }
    template <typename I> IMPLOT_INLINE ImPlotPoint operator()(I idx) const {
        idx = idx % (Count - 1);
        return Getter(idx);
    }
    const _Getter Getter;
    const int Count;
};

template <typename T>
struct GetterError {
    GetterError(const T* xs, const T* ys, const T* neg, const T* pos, int count, int offset, int stride) :
        Xs(xs),
        Ys(ys),
        Neg(neg),
        Pos(pos),
        Count(count),
        Offset(count ? ImPosMod(offset, count) : 0),
        Stride(stride)
    { }
    template <typename I> IMPLOT_INLINE ImPlotPointError operator()(I idx) const {
        return ImPlotPointError((double)IndexData(Xs,  idx, Count, Offset, Stride),
                                (double)IndexData(Ys,  idx, Count, Offset, Stride),
                                (double)IndexData(Neg, idx, Count, Offset, Stride),
                                (double)IndexData(Pos, idx, Count, Offset, Stride));
    }
    const T* const Xs;
    const T* const Ys;
    const T* const Neg;
    const T* const Pos;
    const int Count;
    const int Offset;
    const int Stride;
};

//-----------------------------------------------------------------------------
// [SECTION] Fitters
//-----------------------------------------------------------------------------

template <typename _Getter1>
struct Fitter1 {
    Fitter1(const _Getter1& getter) : Getter(getter) { }
    void Fit(ImPlotAxis& x_axis, ImPlotAxis& y_axis) const {
        for (int i = 0; i < Getter.Count; ++i) {
            ImPlotPoint p = Getter(i);
            x_axis.ExtendFitWith(y_axis, p.x, p.y);
            y_axis.ExtendFitWith(x_axis, p.y, p.x);
        }
    }
    const _Getter1& Getter;
};

template <typename _Getter1>
struct FitterX {
    FitterX(const _Getter1& getter) : Getter(getter) { }
    void Fit(ImPlotAxis& x_axis, ImPlotAxis&) const {
        for (int i = 0; i < Getter.Count; ++i) {
            ImPlotPoint p = Getter(i);
            x_axis.ExtendFit(p.x);
        }
    }
    const _Getter1& Getter;
};

template <typename _Getter1>
struct FitterY {
    FitterY(const _Getter1& getter) : Getter(getter) { }
    void Fit(ImPlotAxis&, ImPlotAxis& y_axis) const {
        for (int i = 0; i < Getter.Count; ++i) {
            ImPlotPoint p = Getter(i);
            y_axis.ExtendFit(p.y);
        }
    }
    const _Getter1& Getter;
};

template <typename _Getter1, typename _Getter2>
struct Fitter2 {
    Fitter2(const _Getter1& getter1, const _Getter2& getter2) : Getter1(getter1), Getter2(getter2) { }
    void Fit(ImPlotAxis& x_axis, ImPlotAxis& y_axis) const {
        for (int i = 0; i < Getter1.Count; ++i) {
            ImPlotPoint p = Getter1(i);
            x_axis.ExtendFitWith(y_axis, p.x, p.y);
            y_axis.ExtendFitWith(x_axis, p.y, p.x);
        }
        for (int i = 0; i < Getter2.Count; ++i) {
            ImPlotPoint p = Getter2(i);
            x_axis.ExtendFitWith(y_axis, p.x, p.y);
            y_axis.ExtendFitWith(x_axis, p.y, p.x);
        }
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
};

template <typename _Getter1, typename _Getter2>
struct FitterBarV {
    FitterBarV(const _Getter1& getter1, const _Getter2& getter2, double width) :
        Getter1(getter1),
        Getter2(getter2),
        HalfWidth(width*0.5)
    { }
    void Fit(ImPlotAxis& x_axis, ImPlotAxis& y_axis) const {
        int count = ImMin(Getter1.Count, Getter2.Count);
        for (int i = 0; i < count; ++i) {
            ImPlotPoint p1 = Getter1(i); p1.x -= HalfWidth;
            ImPlotPoint p2 = Getter2(i); p2.x += HalfWidth;
            x_axis.ExtendFitWith(y_axis, p1.x, p1.y);
            y_axis.ExtendFitWith(x_axis, p1.y, p1.x);
            x_axis.ExtendFitWith(y_axis, p2.x, p2.y);
            y_axis.ExtendFitWith(x_axis, p2.y, p2.x);
        }
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const double    HalfWidth;
};

template <typename _Getter1, typename _Getter2>
struct FitterBarH {
    FitterBarH(const _Getter1& getter1, const _Getter2& getter2, double height) :
        Getter1(getter1),
        Getter2(getter2),
        HalfHeight(height*0.5)
    { }
    void Fit(ImPlotAxis& x_axis, ImPlotAxis& y_axis) const {
        int count = ImMin(Getter1.Count, Getter2.Count);
        for (int i = 0; i < count; ++i) {
            ImPlotPoint p1 = Getter1(i); p1.y -= HalfHeight;
            ImPlotPoint p2 = Getter2(i); p2.y += HalfHeight;
            x_axis.ExtendFitWith(y_axis, p1.x, p1.y);
            y_axis.ExtendFitWith(x_axis, p1.y, p1.x);
            x_axis.ExtendFitWith(y_axis, p2.x, p2.y);
            y_axis.ExtendFitWith(x_axis, p2.y, p2.x);
        }
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const double    HalfHeight;
};

struct FitterRect {
    FitterRect(const ImPlotPoint& pmin, const ImPlotPoint& pmax) :
        Pmin(pmin),
        Pmax(pmax)
    { }
    FitterRect(const ImPlotRect& rect) :
        FitterRect(rect.Min(), rect.Max())
    { }
    void Fit(ImPlotAxis& x_axis, ImPlotAxis& y_axis) const {
        x_axis.ExtendFitWith(y_axis, Pmin.x, Pmin.y);
        y_axis.ExtendFitWith(x_axis, Pmin.y, Pmin.x);
        x_axis.ExtendFitWith(y_axis, Pmax.x, Pmax.y);
        y_axis.ExtendFitWith(x_axis, Pmax.y, Pmax.x);
    }
    const ImPlotPoint Pmin;
    const ImPlotPoint Pmax;
};

//-----------------------------------------------------------------------------
// [SECTION] Transformers
//-----------------------------------------------------------------------------

struct Transformer1 {
    Transformer1(double pixMin, double pltMin, double pltMax, double m, double scaMin, double scaMax, ImPlotTransform fwd, void* data) :
        ScaMin(scaMin),
        ScaMax(scaMax),
        PltMin(pltMin),
        PltMax(pltMax),
        PixMin(pixMin),
        M(m),
        TransformFwd(fwd),
        TransformData(data)
    { }

    template <typename T> IMPLOT_INLINE float operator()(T p) const {
        if (TransformFwd != nullptr) {
            double s = TransformFwd(p, TransformData);
            double t = (s - ScaMin) / (ScaMax - ScaMin);
            p = PltMin + (PltMax - PltMin) * t;
        }
        return (float)(PixMin + M * (p - PltMin));
    }

    double ScaMin, ScaMax, PltMin, PltMax, PixMin, M;
    ImPlotTransform TransformFwd;
    void*           TransformData;
};

struct Transformer2 {
    Transformer2(const ImPlotAxis& x_axis, const ImPlotAxis& y_axis) :
        Tx(x_axis.PixelMin,
           x_axis.Range.Min,
           x_axis.Range.Max,
           x_axis.ScaleToPixel,
           x_axis.ScaleMin,
           x_axis.ScaleMax,
           x_axis.TransformForward,
           x_axis.TransformData),
        Ty(y_axis.PixelMin,
           y_axis.Range.Min,
           y_axis.Range.Max,
           y_axis.ScaleToPixel,
           y_axis.ScaleMin,
           y_axis.ScaleMax,
           y_axis.TransformForward,
           y_axis.TransformData)
    { }

    Transformer2(const ImPlotPlot& plot) :
        Transformer2(plot.Axes[plot.CurrentX], plot.Axes[plot.CurrentY])
    { }

    Transformer2() :
        Transformer2(*GImPlot->CurrentPlot)
    { }

    template <typename P> IMPLOT_INLINE ImVec2 operator()(const P& plt) const {
        ImVec2 out;
        out.x = Tx(plt.x);
        out.y = Ty(plt.y);
        return out;
    }

    template <typename T> IMPLOT_INLINE ImVec2 operator()(T x, T y) const {
        ImVec2 out;
        out.x = Tx(x);
        out.y = Ty(y);
        return out;
    }

    Transformer1 Tx;
    Transformer1 Ty;
};

//-----------------------------------------------------------------------------
// [SECTION] Renderers
//-----------------------------------------------------------------------------

struct RendererBase {
    RendererBase(int prims, int idx_consumed, int vtx_consumed) :
        Prims(prims),
        IdxConsumed(idx_consumed),
        VtxConsumed(vtx_consumed)
    { }
    const int Prims;
    Transformer2 Transformer;
    const int IdxConsumed;
    const int VtxConsumed;
};

template <class _Getter>
struct RendererLineStrip : RendererBase {
    RendererLineStrip(const _Getter& getter, ImU32 col, float weight) :
        RendererBase(getter.Count - 1, 6, 4),
        Getter(getter),
        Col(col),
        HalfWeight(ImMax(1.0f,weight)*0.5f)
    {
        P1 = this->Transformer(Getter(0));
    }
    void Init(ImDrawList& draw_list) const {
        GetLineRenderProps(draw_list, HalfWeight, UV0, UV1);
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P2 = this->Transformer(Getter(prim + 1));
        if (!cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2)))) {
            P1 = P2;
            return false;
        }
        PrimLine(draw_list,P1,P2,HalfWeight,Col,UV0,UV1);
        P1 = P2;
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    mutable float HalfWeight;
    mutable ImVec2 P1;
    mutable ImVec2 UV0;
    mutable ImVec2 UV1;
};

template <class _Getter>
struct RendererLineStripSkip : RendererBase {
    RendererLineStripSkip(const _Getter& getter, ImU32 col, float weight) :
        RendererBase(getter.Count - 1, 6, 4),
        Getter(getter),
        Col(col),
        HalfWeight(ImMax(1.0f,weight)*0.5f)
    {
        P1 = this->Transformer(Getter(0));
    }
    void Init(ImDrawList& draw_list) const {
        GetLineRenderProps(draw_list, HalfWeight, UV0, UV1);
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P2 = this->Transformer(Getter(prim + 1));
        if (!cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2)))) {
            if (!ImNan(P2.x) && !ImNan(P2.y))
                P1 = P2;
            return false;
        }
        PrimLine(draw_list,P1,P2,HalfWeight,Col,UV0,UV1);
        if (!ImNan(P2.x) && !ImNan(P2.y))
            P1 = P2;
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    mutable float HalfWeight;
    mutable ImVec2 P1;
    mutable ImVec2 UV0;
    mutable ImVec2 UV1;
};

template <class _Getter>
struct RendererLineSegments1 : RendererBase {
    RendererLineSegments1(const _Getter& getter, ImU32 col, float weight) :
        RendererBase(getter.Count / 2, 6, 4),
        Getter(getter),
        Col(col),
        HalfWeight(ImMax(1.0f,weight)*0.5f)
    { }
    void Init(ImDrawList& draw_list) const {
        GetLineRenderProps(draw_list, HalfWeight, UV0, UV1);
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P1 = this->Transformer(Getter(prim*2+0));
        ImVec2 P2 = this->Transformer(Getter(prim*2+1));
        if (!cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2))))
            return false;
        PrimLine(draw_list,P1,P2,HalfWeight,Col,UV0,UV1);
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    mutable float HalfWeight;
    mutable ImVec2 UV0;
    mutable ImVec2 UV1;
};

template <class _Getter1, class _Getter2>
struct RendererLineSegments2 : RendererBase {
    RendererLineSegments2(const _Getter1& getter1, const _Getter2& getter2, ImU32 col, float weight) :
        RendererBase(ImMin(getter1.Count, getter1.Count), 6, 4),
        Getter1(getter1),
        Getter2(getter2),
        Col(col),
        HalfWeight(ImMax(1.0f,weight)*0.5f)
    {}
    void Init(ImDrawList& draw_list) const {
        GetLineRenderProps(draw_list, HalfWeight, UV0, UV1);
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P1 = this->Transformer(Getter1(prim));
        ImVec2 P2 = this->Transformer(Getter2(prim));
        if (!cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2))))
            return false;
        PrimLine(draw_list,P1,P2,HalfWeight,Col,UV0,UV1);
        return true;
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const ImU32 Col;
    mutable float HalfWeight;
    mutable ImVec2 UV0;
    mutable ImVec2 UV1;
};

template <class _Getter1, class _Getter2>
struct RendererBarsFillV : RendererBase {
    RendererBarsFillV(const _Getter1& getter1, const _Getter2& getter2, ImU32 col, double width) :
        RendererBase(ImMin(getter1.Count, getter1.Count), 6, 4),
        Getter1(getter1),
        Getter2(getter2),
        Col(col),
        HalfWidth(width/2)
    {}
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImPlotPoint p1 = Getter1(prim);
        ImPlotPoint p2 = Getter2(prim);
        p1.x += HalfWidth;
        p2.x -= HalfWidth;
        ImVec2 P1 = this->Transformer(p1);
        ImVec2 P2 = this->Transformer(p2);
        float width_px = ImAbs(P1.x-P2.x);
        if (width_px < 1.0f) {
            P1.x += P1.x > P2.x ? (1-width_px) / 2 : (width_px-1) / 2;
            P2.x += P2.x > P1.x ? (1-width_px) / 2 : (width_px-1) / 2;
        }
        ImVec2 PMin = ImMin(P1, P2);
        ImVec2 PMax = ImMax(P1, P2);
        if (!cull_rect.Overlaps(ImRect(PMin, PMax)))
            return false;
        PrimRectFill(draw_list,PMin,PMax,Col,UV);
        return true;
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const ImU32 Col;
    const double HalfWidth;
    mutable ImVec2 UV;
};

template <class _Getter1, class _Getter2>
struct RendererBarsFillH : RendererBase {
    RendererBarsFillH(const _Getter1& getter1, const _Getter2& getter2, ImU32 col, double height) :
        RendererBase(ImMin(getter1.Count, getter1.Count), 6, 4),
        Getter1(getter1),
        Getter2(getter2),
        Col(col),
        HalfHeight(height/2)
    {}
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImPlotPoint p1 = Getter1(prim);
        ImPlotPoint p2 = Getter2(prim);
        p1.y += HalfHeight;
        p2.y -= HalfHeight;
        ImVec2 P1 = this->Transformer(p1);
        ImVec2 P2 = this->Transformer(p2);
        float height_px = ImAbs(P1.y-P2.y);
        if (height_px < 1.0f) {
            P1.y += P1.y > P2.y ? (1-height_px) / 2 : (height_px-1) / 2;
            P2.y += P2.y > P1.y ? (1-height_px) / 2 : (height_px-1) / 2;
        }
        ImVec2 PMin = ImMin(P1, P2);
        ImVec2 PMax = ImMax(P1, P2);
        if (!cull_rect.Overlaps(ImRect(PMin, PMax)))
            return false;
        PrimRectFill(draw_list,PMin,PMax,Col,UV);
        return true;
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const ImU32 Col;
    const double HalfHeight;
    mutable ImVec2 UV;
};

template <class _Getter1, class _Getter2>
struct RendererBarsLineV : RendererBase {
    RendererBarsLineV(const _Getter1& getter1, const _Getter2& getter2, ImU32 col, double width, float weight) :
        RendererBase(ImMin(getter1.Count, getter1.Count), 24, 8),
        Getter1(getter1),
        Getter2(getter2),
        Col(col),
        HalfWidth(width/2),
        Weight(weight)
    {}
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImPlotPoint p1 = Getter1(prim);
        ImPlotPoint p2 = Getter2(prim);
        p1.x += HalfWidth;
        p2.x -= HalfWidth;
        ImVec2 P1 = this->Transformer(p1);
        ImVec2 P2 = this->Transformer(p2);
        float width_px = ImAbs(P1.x-P2.x);
        if (width_px < 1.0f) {
            P1.x += P1.x > P2.x ? (1-width_px) / 2 : (width_px-1) / 2;
            P2.x += P2.x > P1.x ? (1-width_px) / 2 : (width_px-1) / 2;
        }
        ImVec2 PMin = ImMin(P1, P2);
        ImVec2 PMax = ImMax(P1, P2);
        if (!cull_rect.Overlaps(ImRect(PMin, PMax)))
            return false;
        PrimRectLine(draw_list,PMin,PMax,Weight,Col,UV);
        return true;
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const ImU32 Col;
    const double HalfWidth;
    const float Weight;
    mutable ImVec2 UV;
};

template <class _Getter1, class _Getter2>
struct RendererBarsLineH : RendererBase {
    RendererBarsLineH(const _Getter1& getter1, const _Getter2& getter2, ImU32 col, double height, float weight) :
        RendererBase(ImMin(getter1.Count, getter1.Count), 24, 8),
        Getter1(getter1),
        Getter2(getter2),
        Col(col),
        HalfHeight(height/2),
        Weight(weight)
    {}
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImPlotPoint p1 = Getter1(prim);
        ImPlotPoint p2 = Getter2(prim);
        p1.y += HalfHeight;
        p2.y -= HalfHeight;
        ImVec2 P1 = this->Transformer(p1);
        ImVec2 P2 = this->Transformer(p2);
        float height_px = ImAbs(P1.y-P2.y);
        if (height_px < 1.0f) {
            P1.y += P1.y > P2.y ? (1-height_px) / 2 : (height_px-1) / 2;
            P2.y += P2.y > P1.y ? (1-height_px) / 2 : (height_px-1) / 2;
        }
        ImVec2 PMin = ImMin(P1, P2);
        ImVec2 PMax = ImMax(P1, P2);
        if (!cull_rect.Overlaps(ImRect(PMin, PMax)))
            return false;
        PrimRectLine(draw_list,PMin,PMax,Weight,Col,UV);
        return true;
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const ImU32 Col;
    const double HalfHeight;
    const float Weight;
    mutable ImVec2 UV;
};


template <class _Getter>
struct RendererStairsPre : RendererBase {
    RendererStairsPre(const _Getter& getter, ImU32 col, float weight) :
        RendererBase(getter.Count - 1, 12, 8),
        Getter(getter),
        Col(col),
        HalfWeight(ImMax(1.0f,weight)*0.5f)
    {
        P1 = this->Transformer(Getter(0));
    }
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P2 = this->Transformer(Getter(prim + 1));
        if (!cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2)))) {
            P1 = P2;
            return false;
        }
        PrimRectFill(draw_list, ImVec2(P1.x - HalfWeight, P1.y), ImVec2(P1.x + HalfWeight, P2.y), Col, UV);
        PrimRectFill(draw_list, ImVec2(P1.x, P2.y + HalfWeight), ImVec2(P2.x, P2.y - HalfWeight), Col, UV);
        P1 = P2;
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    mutable float HalfWeight;
    mutable ImVec2 P1;
    mutable ImVec2 UV;
};

template <class _Getter>
struct RendererStairsPost : RendererBase {
    RendererStairsPost(const _Getter& getter, ImU32 col, float weight) :
        RendererBase(getter.Count - 1, 12, 8),
        Getter(getter),
        Col(col),
        HalfWeight(ImMax(1.0f,weight) * 0.5f)
    {
        P1 = this->Transformer(Getter(0));
    }
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P2 = this->Transformer(Getter(prim + 1));
        if (!cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2)))) {
            P1 = P2;
            return false;
        }
        PrimRectFill(draw_list, ImVec2(P1.x, P1.y + HalfWeight), ImVec2(P2.x, P1.y - HalfWeight), Col, UV);
        PrimRectFill(draw_list, ImVec2(P2.x - HalfWeight, P2.y), ImVec2(P2.x + HalfWeight, P1.y), Col, UV);
        P1 = P2;
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    mutable float HalfWeight;
    mutable ImVec2 P1;
    mutable ImVec2 UV;
};

template <class _Getter>
struct RendererStairsPreShaded : RendererBase {
    RendererStairsPreShaded(const _Getter& getter, ImU32 col) :
        RendererBase(getter.Count - 1, 6, 4),
        Getter(getter),
        Col(col)
    {
        P1 = this->Transformer(Getter(0));
        Y0 = this->Transformer(ImPlotPoint(0,0)).y;
    }
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P2 = this->Transformer(Getter(prim + 1));
        ImVec2 PMin(ImMin(P1.x, P2.x), ImMin(Y0, P2.y));
        ImVec2 PMax(ImMax(P1.x, P2.x), ImMax(Y0, P2.y));
        if (!cull_rect.Overlaps(ImRect(PMin, PMax))) {
            P1 = P2;
            return false;
        }
        PrimRectFill(draw_list, PMin, PMax, Col, UV);
        P1 = P2;
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    float Y0;
    mutable ImVec2 P1;
    mutable ImVec2 UV;
};

template <class _Getter>
struct RendererStairsPostShaded : RendererBase {
    RendererStairsPostShaded(const _Getter& getter, ImU32 col) :
        RendererBase(getter.Count - 1, 6, 4),
        Getter(getter),
        Col(col)
    {
        P1 = this->Transformer(Getter(0));
        Y0 = this->Transformer(ImPlotPoint(0,0)).y;
    }
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P2 = this->Transformer(Getter(prim + 1));
        ImVec2 PMin(ImMin(P1.x, P2.x), ImMin(P1.y, Y0));
        ImVec2 PMax(ImMax(P1.x, P2.x), ImMax(P1.y, Y0));
        if (!cull_rect.Overlaps(ImRect(PMin, PMax))) {
            P1 = P2;
            return false;
        }
        PrimRectFill(draw_list, PMin, PMax, Col, UV);
        P1 = P2;
        return true;
    }
    const _Getter& Getter;
    const ImU32 Col;
    float Y0;
    mutable ImVec2 P1;
    mutable ImVec2 UV;
};



template <class _Getter1, class _Getter2>
struct RendererShaded : RendererBase {
    RendererShaded(const _Getter1& getter1, const _Getter2& getter2, ImU32 col) :
        RendererBase(ImMin(getter1.Count, getter2.Count) - 1, 6, 5),
        Getter1(getter1),
        Getter2(getter2),
        Col(col)
    {
        P11 = this->Transformer(Getter1(0));
        P12 = this->Transformer(Getter2(0));
    }
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 P21 = this->Transformer(Getter1(prim+1));
        ImVec2 P22 = this->Transformer(Getter2(prim+1));
        ImRect rect(ImMin(ImMin(ImMin(P11,P12),P21),P22), ImMax(ImMax(ImMax(P11,P12),P21),P22));
        if (!cull_rect.Overlaps(rect)) {
            P11 = P21;
            P12 = P22;
            return false;
        }
        const int intersect = (P11.y > P12.y && P22.y > P21.y) || (P12.y > P11.y && P21.y > P22.y);
        const ImVec2 intersection = intersect == 0 ? ImVec2(0,0) : Intersection(P11,P21,P12,P22);
        draw_list._VtxWritePtr[0].pos = P11;
        draw_list._VtxWritePtr[0].uv  = UV;
        draw_list._VtxWritePtr[0].col = Col;
        draw_list._VtxWritePtr[1].pos = P21;
        draw_list._VtxWritePtr[1].uv  = UV;
        draw_list._VtxWritePtr[1].col = Col;
        draw_list._VtxWritePtr[2].pos = intersection;
        draw_list._VtxWritePtr[2].uv  = UV;
        draw_list._VtxWritePtr[2].col = Col;
        draw_list._VtxWritePtr[3].pos = P12;
        draw_list._VtxWritePtr[3].uv  = UV;
        draw_list._VtxWritePtr[3].col = Col;
        draw_list._VtxWritePtr[4].pos = P22;
        draw_list._VtxWritePtr[4].uv  = UV;
        draw_list._VtxWritePtr[4].col = Col;
        draw_list._VtxWritePtr += 5;
        draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
        draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1 + intersect);
        draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
        draw_list._IdxWritePtr[3] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
        draw_list._IdxWritePtr[4] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 4);
        draw_list._IdxWritePtr[5] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3 - intersect);
        draw_list._IdxWritePtr += 6;
        draw_list._VtxCurrentIdx += 5;
        P11 = P21;
        P12 = P22;
        return true;
    }
    const _Getter1& Getter1;
    const _Getter2& Getter2;
    const ImU32 Col;
    mutable ImVec2 P11;
    mutable ImVec2 P12;
    mutable ImVec2 UV;
};

struct RectC {
    ImPlotPoint Pos;
    ImPlotPoint HalfSize;
    ImU32 Color;
};

template <typename _Getter>
struct RendererRectC : RendererBase {
    RendererRectC(const _Getter& getter) :
        RendererBase(getter.Count, 6, 4),
        Getter(getter)
    {}
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        RectC rect = Getter(prim);
        ImVec2 P1 = this->Transformer(rect.Pos.x - rect.HalfSize.x , rect.Pos.y - rect.HalfSize.y);
        ImVec2 P2 = this->Transformer(rect.Pos.x + rect.HalfSize.x , rect.Pos.y + rect.HalfSize.y);
        if ((rect.Color & IM_COL32_A_MASK) == 0 || !cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2))))
            return false;
        PrimRectFill(draw_list,P1,P2,rect.Color,UV);
        return true;
    }
    const _Getter& Getter;
    mutable ImVec2 UV;
};

//-----------------------------------------------------------------------------
// [SECTION] RenderPrimitives
//-----------------------------------------------------------------------------

/// Renders primitive shapes in bulk as efficiently as possible.
template <class _Renderer>
void RenderPrimitivesEx(const _Renderer& renderer, ImDrawList& draw_list, const ImRect& cull_rect) {
    unsigned int prims        = renderer.Prims;
    unsigned int prims_culled = 0;
    unsigned int idx          = 0;
    renderer.Init(draw_list);
    while (prims) {
        // find how many can be reserved up to end of current draw command's limit
        unsigned int cnt = ImMin(prims, (MaxIdx<ImDrawIdx>::Value - draw_list._VtxCurrentIdx) / renderer.VtxConsumed);
        // make sure at least this many elements can be rendered to avoid situations where at the end of buffer this slow path is not taken all the time
        if (cnt >= ImMin(64u, prims)) {
            if (prims_culled >= cnt)
                prims_culled -= cnt; // reuse previous reservation
            else {
                // add more elements to previous reservation
                draw_list.PrimReserve((cnt - prims_culled) * renderer.IdxConsumed, (cnt - prims_culled) * renderer.VtxConsumed);
                prims_culled = 0;
            }
        }
        else
        {
            if (prims_culled > 0) {
                draw_list.PrimUnreserve(prims_culled * renderer.IdxConsumed, prims_culled * renderer.VtxConsumed);
                prims_culled = 0;
            }
            cnt = ImMin(prims, (MaxIdx<ImDrawIdx>::Value - 0/*draw_list._VtxCurrentIdx*/) / renderer.VtxConsumed);
            // reserve new draw command
            draw_list.PrimReserve(cnt * renderer.IdxConsumed, cnt * renderer.VtxConsumed);
        }
        prims -= cnt;
        for (unsigned int ie = idx + cnt; idx != ie; ++idx) {
            if (!renderer.Render(draw_list, cull_rect, idx))
                prims_culled++;
        }
    }
    if (prims_culled > 0)
        draw_list.PrimUnreserve(prims_culled * renderer.IdxConsumed, prims_culled * renderer.VtxConsumed);
}

template <template <class> class _Renderer, class _Getter, typename ...Args>
void RenderPrimitives1(const _Getter& getter, Args... args) {
    ImDrawList& draw_list = *GetPlotDrawList();
    const ImRect& cull_rect = GetCurrentPlot()->PlotRect;
    RenderPrimitivesEx(_Renderer<_Getter>(getter,args...), draw_list, cull_rect);
}

template <template <class,class> class _Renderer, class _Getter1, class _Getter2, typename ...Args>
void RenderPrimitives2(const _Getter1& getter1, const _Getter2& getter2, Args... args) {
    ImDrawList& draw_list = *GetPlotDrawList();
    const ImRect& cull_rect = GetCurrentPlot()->PlotRect;
    RenderPrimitivesEx(_Renderer<_Getter1,_Getter2>(getter1,getter2,args...), draw_list, cull_rect);
}

//-----------------------------------------------------------------------------
// [SECTION] Markers
//-----------------------------------------------------------------------------

template <class _Getter>
struct RendererMarkersFill : RendererBase {
    RendererMarkersFill(const _Getter& getter, const ImVec2* marker, int count, float size, ImU32 col) :
        RendererBase(getter.Count, (count-2)*3, count),
        Getter(getter),
        Marker(marker),
        Count(count),
        Size(size),
        Col(col)
    { }
    void Init(ImDrawList& draw_list) const {
        UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 p = this->Transformer(Getter(prim));
        if (p.x >= cull_rect.Min.x && p.y >= cull_rect.Min.y && p.x <= cull_rect.Max.x && p.y <= cull_rect.Max.y) {
            for (int i = 0; i < Count; i++) {
                draw_list._VtxWritePtr[0].pos.x = p.x + Marker[i].x * Size;
                draw_list._VtxWritePtr[0].pos.y = p.y + Marker[i].y * Size;
                draw_list._VtxWritePtr[0].uv = UV;
                draw_list._VtxWritePtr[0].col = Col;
                draw_list._VtxWritePtr++;
            }
            for (int i = 2; i < Count; i++) {
                draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
                draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + i - 1);
                draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + i);
                draw_list._IdxWritePtr += 3;
            }
            draw_list._VtxCurrentIdx += (ImDrawIdx)Count;
            return true;
        }
        return false;
    }
    const _Getter& Getter;
    const ImVec2* Marker;
    const int Count;
    const float Size;
    const ImU32 Col;
    mutable ImVec2 UV;
};


template <class _Getter>
struct RendererMarkersLine : RendererBase {
    RendererMarkersLine(const _Getter& getter, const ImVec2* marker, int count, float size, float weight, ImU32 col) :
        RendererBase(getter.Count, count/2*6, count/2*4),
        Getter(getter),
        Marker(marker),
        Count(count),
        HalfWeight(ImMax(1.0f,weight)*0.5f),
        Size(size),
        Col(col)
    { }
    void Init(ImDrawList& draw_list) const {
        GetLineRenderProps(draw_list, HalfWeight, UV0, UV1);
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
        ImVec2 p = this->Transformer(Getter(prim));
        if (p.x >= cull_rect.Min.x && p.y >= cull_rect.Min.y && p.x <= cull_rect.Max.x && p.y <= cull_rect.Max.y) {
            for (int i = 0; i < Count; i = i + 2) {
                ImVec2 p1(p.x + Marker[i].x * Size, p.y + Marker[i].y * Size);
                ImVec2 p2(p.x + Marker[i+1].x * Size, p.y + Marker[i+1].y * Size);
                PrimLine(draw_list, p1, p2, HalfWeight, Col, UV0, UV1);
            }
            return true;
        }
        return false;
    }
    const _Getter& Getter;
    const ImVec2* Marker;
    const int Count;
    mutable float HalfWeight;
    const float Size;
    const ImU32 Col;
    mutable ImVec2 UV0;
    mutable ImVec2 UV1;
};

static const ImVec2 MARKER_FILL_CIRCLE[10]  = {ImVec2(1.0f, 0.0f), ImVec2(0.809017f, 0.58778524f),ImVec2(0.30901697f, 0.95105654f),ImVec2(-0.30901703f, 0.9510565f),ImVec2(-0.80901706f, 0.5877852f),ImVec2(-1.0f, 0.0f),ImVec2(-0.80901694f, -0.58778536f),ImVec2(-0.3090171f, -0.9510565f),ImVec2(0.30901712f, -0.9510565f),ImVec2(0.80901694f, -0.5877853f)};
static const ImVec2 MARKER_FILL_SQUARE[4]   = {ImVec2(SQRT_1_2,SQRT_1_2), ImVec2(SQRT_1_2,-SQRT_1_2), ImVec2(-SQRT_1_2,-SQRT_1_2), ImVec2(-SQRT_1_2,SQRT_1_2)};
static const ImVec2 MARKER_FILL_DIAMOND[4]  = {ImVec2(1, 0), ImVec2(0, -1), ImVec2(-1, 0), ImVec2(0, 1)};
static const ImVec2 MARKER_FILL_UP[3]       = {ImVec2(SQRT_3_2,0.5f),ImVec2(0,-1),ImVec2(-SQRT_3_2,0.5f)};
static const ImVec2 MARKER_FILL_DOWN[3]     = {ImVec2(SQRT_3_2,-0.5f),ImVec2(0,1),ImVec2(-SQRT_3_2,-0.5f)};
static const ImVec2 MARKER_FILL_LEFT[3]     = {ImVec2(-1,0), ImVec2(0.5, SQRT_3_2), ImVec2(0.5, -SQRT_3_2)};
static const ImVec2 MARKER_FILL_RIGHT[3]    = {ImVec2(1,0), ImVec2(-0.5, SQRT_3_2), ImVec2(-0.5, -SQRT_3_2)};

static const ImVec2 MARKER_LINE_CIRCLE[20]  = {
    ImVec2(1.0f, 0.0f),
    ImVec2(0.809017f, 0.58778524f),
    ImVec2(0.809017f, 0.58778524f),
    ImVec2(0.30901697f, 0.95105654f),
    ImVec2(0.30901697f, 0.95105654f),
    ImVec2(-0.30901703f, 0.9510565f),
    ImVec2(-0.30901703f, 0.9510565f),
    ImVec2(-0.80901706f, 0.5877852f),
    ImVec2(-0.80901706f, 0.5877852f),
    ImVec2(-1.0f, 0.0f),
    ImVec2(-1.0f, 0.0f),
    ImVec2(-0.80901694f, -0.58778536f),
    ImVec2(-0.80901694f, -0.58778536f),
    ImVec2(-0.3090171f, -0.9510565f),
    ImVec2(-0.3090171f, -0.9510565f),
    ImVec2(0.30901712f, -0.9510565f),
    ImVec2(0.30901712f, -0.9510565f),
    ImVec2(0.80901694f, -0.5877853f),
    ImVec2(0.80901694f, -0.5877853f),
    ImVec2(1.0f, 0.0f)
};
static const ImVec2 MARKER_LINE_SQUARE[8]   = {ImVec2(SQRT_1_2,SQRT_1_2), ImVec2(SQRT_1_2,-SQRT_1_2), ImVec2(SQRT_1_2,-SQRT_1_2), ImVec2(-SQRT_1_2,-SQRT_1_2), ImVec2(-SQRT_1_2,-SQRT_1_2), ImVec2(-SQRT_1_2,SQRT_1_2), ImVec2(-SQRT_1_2,SQRT_1_2), ImVec2(SQRT_1_2,SQRT_1_2)};
static const ImVec2 MARKER_LINE_DIAMOND[8]  = {ImVec2(1, 0), ImVec2(0, -1), ImVec2(0, -1), ImVec2(-1, 0), ImVec2(-1, 0), ImVec2(0, 1), ImVec2(0, 1), ImVec2(1, 0)};
static const ImVec2 MARKER_LINE_UP[6]       = {ImVec2(SQRT_3_2,0.5f), ImVec2(0,-1),ImVec2(0,-1),ImVec2(-SQRT_3_2,0.5f),ImVec2(-SQRT_3_2,0.5f),ImVec2(SQRT_3_2,0.5f)};
static const ImVec2 MARKER_LINE_DOWN[6]     = {ImVec2(SQRT_3_2,-0.5f),ImVec2(0,1),ImVec2(0,1),ImVec2(-SQRT_3_2,-0.5f), ImVec2(-SQRT_3_2,-0.5f), ImVec2(SQRT_3_2,-0.5f)};
static const ImVec2 MARKER_LINE_LEFT[6]     = {ImVec2(-1,0), ImVec2(0.5, SQRT_3_2),  ImVec2(0.5, SQRT_3_2),  ImVec2(0.5, -SQRT_3_2) , ImVec2(0.5, -SQRT_3_2) , ImVec2(-1,0) };
static const ImVec2 MARKER_LINE_RIGHT[6]    = {ImVec2(1,0),  ImVec2(-0.5, SQRT_3_2), ImVec2(-0.5, SQRT_3_2), ImVec2(-0.5, -SQRT_3_2), ImVec2(-0.5, -SQRT_3_2), ImVec2(1,0) };
static const ImVec2 MARKER_LINE_ASTERISK[6] = {ImVec2(-SQRT_3_2, -0.5f), ImVec2(SQRT_3_2, 0.5f),  ImVec2(-SQRT_3_2, 0.5f), ImVec2(SQRT_3_2, -0.5f), ImVec2(0, -1), ImVec2(0, 1)};
static const ImVec2 MARKER_LINE_PLUS[4]     = {ImVec2(-1, 0), ImVec2(1, 0), ImVec2(0, -1), ImVec2(0, 1)};
static const ImVec2 MARKER_LINE_CROSS[4]    = {ImVec2(-SQRT_1_2,-SQRT_1_2),ImVec2(SQRT_1_2,SQRT_1_2),ImVec2(SQRT_1_2,-SQRT_1_2),ImVec2(-SQRT_1_2,SQRT_1_2)};

template <typename _Getter>
void RenderMarkers(const _Getter& getter, ImPlotMarker marker, float size, bool rend_fill, ImU32 col_fill, bool rend_line, ImU32 col_line, float weight) {
    if (rend_fill) {
        switch (marker) {
            case ImPlotMarker_Circle  : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_CIRCLE,10,size,col_fill); break;
            case ImPlotMarker_Square  : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_SQUARE, 4,size,col_fill); break;
            case ImPlotMarker_Diamond : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_DIAMOND,4,size,col_fill); break;
            case ImPlotMarker_Up      : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_UP,     3,size,col_fill); break;
            case ImPlotMarker_Down    : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_DOWN,   3,size,col_fill); break;
            case ImPlotMarker_Left    : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_LEFT,   3,size,col_fill); break;
            case ImPlotMarker_Right   : RenderPrimitives1<RendererMarkersFill>(getter,MARKER_FILL_RIGHT,  3,size,col_fill); break;
        }
    }
    if (rend_line) {
        switch (marker) {
            case ImPlotMarker_Circle    : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_CIRCLE, 20,size,weight,col_line); break;
            case ImPlotMarker_Square    : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_SQUARE,  8,size,weight,col_line); break;
            case ImPlotMarker_Diamond   : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_DIAMOND, 8,size,weight,col_line); break;
            case ImPlotMarker_Up        : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_UP,      6,size,weight,col_line); break;
            case ImPlotMarker_Down      : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_DOWN,    6,size,weight,col_line); break;
            case ImPlotMarker_Left      : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_LEFT,    6,size,weight,col_line); break;
            case ImPlotMarker_Right     : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_RIGHT,   6,size,weight,col_line); break;
            case ImPlotMarker_Asterisk  : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_ASTERISK,6,size,weight,col_line); break;
            case ImPlotMarker_Plus      : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_PLUS,    4,size,weight,col_line); break;
            case ImPlotMarker_Cross     : RenderPrimitives1<RendererMarkersLine>(getter,MARKER_LINE_CROSS,   4,size,weight,col_line); break;
        }
    }
}

//-----------------------------------------------------------------------------
// [SECTION] PlotLine
//-----------------------------------------------------------------------------

template <typename _Getter>
void PlotLineEx(const char* label_id, const _Getter& getter, ImPlotLineFlags flags) {
    if (BeginItemEx(label_id, Fitter1<_Getter>(getter), flags, ImPlotCol_Line)) {
        if (getter.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        if (getter.Count > 1) {
            if (ImHasFlag(flags, ImPlotLineFlags_Shaded) && s.RenderFill) {
                const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]);
                GetterOverrideY<_Getter> getter2(getter, 0);
                RenderPrimitives2<RendererShaded>(getter,getter2,col_fill);
            }
            if (s.RenderLine) {
                const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
                if (ImHasFlag(flags,ImPlotLineFlags_Segments)) {
                    RenderPrimitives1<RendererLineSegments1>(getter,col_line,s.LineWeight);
                }
                else if (ImHasFlag(flags, ImPlotLineFlags_Loop)) {
                    if (ImHasFlag(flags, ImPlotLineFlags_SkipNaN))
                        RenderPrimitives1<RendererLineStripSkip>(GetterLoop<_Getter>(getter),col_line,s.LineWeight);
                    else
                        RenderPrimitives1<RendererLineStrip>(GetterLoop<_Getter>(getter),col_line,s.LineWeight);
                }
                else {
                    if (ImHasFlag(flags, ImPlotLineFlags_SkipNaN))
                        RenderPrimitives1<RendererLineStripSkip>(getter,col_line,s.LineWeight);
                    else
                        RenderPrimitives1<RendererLineStrip>(getter,col_line,s.LineWeight);
                }
            }
        }
        // render markers
        if (s.Marker != ImPlotMarker_None) {
            if (ImHasFlag(flags, ImPlotLineFlags_NoClip)) {
                PopPlotClipRect();
                PushPlotClipRect(s.MarkerSize);
            }
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerOutline]);
            const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerFill]);
            RenderMarkers<_Getter>(getter, s.Marker, s.MarkerSize, s.RenderMarkerFill, col_fill, s.RenderMarkerLine, col_line, s.MarkerWeight);
        }
        EndItem();
    }
}

template <typename T>
void PlotLine(const char* label_id, const T* values, int count, double xscale, double x0, ImPlotLineFlags flags, int offset, int stride) {
    GetterXY<IndexerLin,IndexerIdx<T>> getter(IndexerLin(xscale,x0),IndexerIdx<T>(values,count,offset,stride),count);
    PlotLineEx(label_id, getter, flags);
}

template <typename T>
void PlotLine(const char* label_id, const T* xs, const T* ys, int count, ImPlotLineFlags flags, int offset, int stride) {
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
    PlotLineEx(label_id, getter, flags);
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotLine<T> (const char* label_id, const T* values, int count, double xscale, double x0, ImPlotLineFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotLine<T>(const char* label_id, const T* xs, const T* ys, int count, ImPlotLineFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

// custom
void PlotLineG(const char* label_id, ImPlotGetter getter_func, void* data, int count, ImPlotLineFlags flags) {
    GetterFuncPtr getter(getter_func,data, count);
    PlotLineEx(label_id, getter, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] PlotScatter
//-----------------------------------------------------------------------------

template <typename Getter>
void PlotScatterEx(const char* label_id, const Getter& getter, ImPlotScatterFlags flags) {
    if (BeginItemEx(label_id, Fitter1<Getter>(getter), flags, ImPlotCol_MarkerOutline)) {
        if (getter.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        ImPlotMarker marker = s.Marker == ImPlotMarker_None ? ImPlotMarker_Circle: s.Marker;
        if (marker != ImPlotMarker_None) {
            if (ImHasFlag(flags,ImPlotScatterFlags_NoClip)) {
                PopPlotClipRect();
                PushPlotClipRect(s.MarkerSize);
            }
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerOutline]);
            const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerFill]);
            RenderMarkers<Getter>(getter, marker, s.MarkerSize, s.RenderMarkerFill, col_fill, s.RenderMarkerLine, col_line, s.MarkerWeight);
        }
        EndItem();
    }
}

template <typename T>
void PlotScatter(const char* label_id, const T* values, int count, double xscale, double x0, ImPlotScatterFlags flags, int offset, int stride) {
    GetterXY<IndexerLin,IndexerIdx<T>> getter(IndexerLin(xscale,x0),IndexerIdx<T>(values,count,offset,stride),count);
    PlotScatterEx(label_id, getter, flags);
}

template <typename T>
void PlotScatter(const char* label_id, const T* xs, const T* ys, int count, ImPlotScatterFlags flags, int offset, int stride) {
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
    return PlotScatterEx(label_id, getter, flags);
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotScatter<T>(const char* label_id, const T* values, int count, double xscale, double x0, ImPlotScatterFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotScatter<T>(const char* label_id, const T* xs, const T* ys, int count, ImPlotScatterFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

// custom
void PlotScatterG(const char* label_id, ImPlotGetter getter_func, void* data, int count, ImPlotScatterFlags flags) {
    GetterFuncPtr getter(getter_func,data, count);
    return PlotScatterEx(label_id, getter, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] PlotStairs
//-----------------------------------------------------------------------------

template <typename Getter>
void PlotStairsEx(const char* label_id, const Getter& getter, ImPlotStairsFlags flags) {
    if (BeginItemEx(label_id, Fitter1<Getter>(getter), flags, ImPlotCol_Line)) {
        if (getter.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        if (getter.Count > 1) {
            if (s.RenderFill && ImHasFlag(flags,ImPlotStairsFlags_Shaded)) {
                const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]);
                if (ImHasFlag(flags, ImPlotStairsFlags_PreStep))
                    RenderPrimitives1<RendererStairsPreShaded>(getter,col_fill);
                else
                    RenderPrimitives1<RendererStairsPostShaded>(getter,col_fill);
            }
            if (s.RenderLine) {
                const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
                if (ImHasFlag(flags, ImPlotStairsFlags_PreStep))
                    RenderPrimitives1<RendererStairsPre>(getter,col_line,s.LineWeight);
                else
                    RenderPrimitives1<RendererStairsPost>(getter,col_line,s.LineWeight);
            }
        }
        // render markers
        if (s.Marker != ImPlotMarker_None) {
            PopPlotClipRect();
            PushPlotClipRect(s.MarkerSize);
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerOutline]);
            const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerFill]);
            RenderMarkers<Getter>(getter, s.Marker, s.MarkerSize, s.RenderMarkerFill, col_fill, s.RenderMarkerLine, col_line, s.MarkerWeight);
        }
        EndItem();
    }
}

template <typename T>
void PlotStairs(const char* label_id, const T* values, int count, double xscale, double x0, ImPlotStairsFlags flags, int offset, int stride) {
    GetterXY<IndexerLin,IndexerIdx<T>> getter(IndexerLin(xscale,x0),IndexerIdx<T>(values,count,offset,stride),count);
    PlotStairsEx(label_id, getter, flags);
}

template <typename T>
void PlotStairs(const char* label_id, const T* xs, const T* ys, int count, ImPlotStairsFlags flags, int offset, int stride) {
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
    return PlotStairsEx(label_id, getter, flags);
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotStairs<T> (const char* label_id, const T* values, int count, double xscale, double x0, ImPlotStairsFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotStairs<T>(const char* label_id, const T* xs, const T* ys, int count, ImPlotStairsFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

// custom
void PlotStairsG(const char* label_id, ImPlotGetter getter_func, void* data, int count, ImPlotStairsFlags flags) {
    GetterFuncPtr getter(getter_func,data, count);
    return PlotStairsEx(label_id, getter, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] PlotShaded
//-----------------------------------------------------------------------------

template <typename Getter1, typename Getter2>
void PlotShadedEx(const char* label_id, const Getter1& getter1, const Getter2& getter2, ImPlotShadedFlags flags) {
    if (BeginItemEx(label_id, Fitter2<Getter1,Getter2>(getter1,getter2), flags, ImPlotCol_Fill)) {
        if (getter1.Count <= 0 || getter2.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        if (s.RenderFill) {
            const ImU32 col = ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]);
            RenderPrimitives2<RendererShaded>(getter1,getter2,col);
        }
        EndItem();
    }
}

template <typename T>
void PlotShaded(const char* label_id, const T* values, int count, double y_ref, double xscale, double x0, ImPlotShadedFlags flags, int offset, int stride) {
    if (!(y_ref > -DBL_MAX))
        y_ref = GetPlotLimits(IMPLOT_AUTO,IMPLOT_AUTO).Y.Min;
    if (!(y_ref < DBL_MAX))
        y_ref = GetPlotLimits(IMPLOT_AUTO,IMPLOT_AUTO).Y.Max;
    GetterXY<IndexerLin,IndexerIdx<T>> getter1(IndexerLin(xscale,x0),IndexerIdx<T>(values,count,offset,stride),count);
    GetterXY<IndexerLin,IndexerConst>  getter2(IndexerLin(xscale,x0),IndexerConst(y_ref),count);
    PlotShadedEx(label_id, getter1, getter2, flags);
}

template <typename T>
void PlotShaded(const char* label_id, const T* xs, const T* ys, int count, double y_ref, ImPlotShadedFlags flags, int offset, int stride) {
    if (y_ref == -HUGE_VAL)
        y_ref = GetPlotLimits(IMPLOT_AUTO,IMPLOT_AUTO).Y.Min;
    if (y_ref == HUGE_VAL)
        y_ref = GetPlotLimits(IMPLOT_AUTO,IMPLOT_AUTO).Y.Max;
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter1(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
    GetterXY<IndexerIdx<T>,IndexerConst>  getter2(IndexerIdx<T>(xs,count,offset,stride),IndexerConst(y_ref),count);
    PlotShadedEx(label_id, getter1, getter2, flags);
}


template <typename T>
void PlotShaded(const char* label_id, const T* xs, const T* ys1, const T* ys2, int count, ImPlotShadedFlags flags, int offset, int stride) {
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter1(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys1,count,offset,stride),count);
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter2(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys2,count,offset,stride),count);
    PlotShadedEx(label_id, getter1, getter2, flags);
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotShaded<T>(const char* label_id, const T* values, int count, double y_ref, double xscale, double x0, ImPlotShadedFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotShaded<T>(const char* label_id, const T* xs, const T* ys, int count, double y_ref, ImPlotShadedFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotShaded<T>(const char* label_id, const T* xs, const T* ys1, const T* ys2, int count, ImPlotShadedFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

// custom
void PlotShadedG(const char* label_id, ImPlotGetter getter_func1, void* data1, ImPlotGetter getter_func2, void* data2, int count, ImPlotShadedFlags flags) {
    GetterFuncPtr getter1(getter_func1, data1, count);
    GetterFuncPtr getter2(getter_func2, data2, count);
    PlotShadedEx(label_id, getter1, getter2, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] PlotBars
//-----------------------------------------------------------------------------

template <typename Getter1, typename Getter2>
void PlotBarsVEx(const char* label_id, const Getter1& getter1, const Getter2 getter2, double width, ImPlotBarsFlags flags) {
    if (BeginItemEx(label_id, FitterBarV<Getter1,Getter2>(getter1,getter2,width), flags, ImPlotCol_Fill)) {
        if (getter1.Count <= 0 || getter2.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]);
        const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
        bool rend_fill = s.RenderFill;
        bool rend_line = s.RenderLine;
        if (rend_fill) {
            RenderPrimitives2<RendererBarsFillV>(getter1,getter2,col_fill,width);
            if (rend_line && col_fill == col_line)
                rend_line = false;
        }
        if (rend_line) {
            RenderPrimitives2<RendererBarsLineV>(getter1,getter2,col_line,width,s.LineWeight);
        }
        EndItem();
    }
}

template <typename Getter1, typename Getter2>
void PlotBarsHEx(const char* label_id, const Getter1& getter1, const Getter2& getter2, double height, ImPlotBarsFlags flags) {
    if (BeginItemEx(label_id, FitterBarH<Getter1,Getter2>(getter1,getter2,height), flags, ImPlotCol_Fill)) {
        if (getter1.Count <= 0 || getter2.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]);
        const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
        bool rend_fill = s.RenderFill;
        bool rend_line = s.RenderLine;
        if (rend_fill) {
            RenderPrimitives2<RendererBarsFillH>(getter1,getter2,col_fill,height);
            if (rend_line && col_fill == col_line)
                rend_line = false;
        }
        if (rend_line) {
            RenderPrimitives2<RendererBarsLineH>(getter1,getter2,col_line,height,s.LineWeight);
        }
        EndItem();
    }
}

template <typename T>
void PlotBars(const char* label_id, const T* values, int count, double bar_size, double shift, ImPlotBarsFlags flags, int offset, int stride) {
    if (ImHasFlag(flags, ImPlotBarsFlags_Horizontal)) {
        GetterXY<IndexerIdx<T>,IndexerLin> getter1(IndexerIdx<T>(values,count,offset,stride),IndexerLin(1.0,shift),count);
        GetterXY<IndexerConst,IndexerLin>  getter2(IndexerConst(0),IndexerLin(1.0,shift),count);
        PlotBarsHEx(label_id, getter1, getter2, bar_size, flags);
    }
    else {
        GetterXY<IndexerLin,IndexerIdx<T>> getter1(IndexerLin(1.0,shift),IndexerIdx<T>(values,count,offset,stride),count);
        GetterXY<IndexerLin,IndexerConst>  getter2(IndexerLin(1.0,shift),IndexerConst(0),count);
        PlotBarsVEx(label_id, getter1, getter2, bar_size, flags);
    }
}

template <typename T>
void PlotBars(const char* label_id, const T* xs, const T* ys, int count, double bar_size, ImPlotBarsFlags flags, int offset, int stride) {
    if (ImHasFlag(flags, ImPlotBarsFlags_Horizontal)) {
        GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter1(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
        GetterXY<IndexerConst, IndexerIdx<T>> getter2(IndexerConst(0),IndexerIdx<T>(ys,count,offset,stride),count);
        PlotBarsHEx(label_id, getter1, getter2, bar_size, flags);
    }
    else {
        GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter1(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
        GetterXY<IndexerIdx<T>,IndexerConst>  getter2(IndexerIdx<T>(xs,count,offset,stride),IndexerConst(0),count);
        PlotBarsVEx(label_id, getter1, getter2, bar_size, flags);
    }
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotBars<T>(const char* label_id, const T* values, int count, double bar_size, double shift, ImPlotBarsFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotBars<T>(const char* label_id, const T* xs, const T* ys, int count, double bar_size, ImPlotBarsFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

void PlotBarsG(const char* label_id, ImPlotGetter getter_func, void* data, int count, double bar_size, ImPlotBarsFlags flags) {
    if (ImHasFlag(flags, ImPlotBarsFlags_Horizontal)) {
        GetterFuncPtr getter1(getter_func, data, count);
        GetterOverrideX<GetterFuncPtr> getter2(getter1,0);
        PlotBarsHEx(label_id, getter1, getter2, bar_size, flags);
    }
    else {
        GetterFuncPtr getter1(getter_func, data, count);
        GetterOverrideY<GetterFuncPtr> getter2(getter1,0);
        PlotBarsVEx(label_id, getter1, getter2, bar_size, flags);
    }
}

//-----------------------------------------------------------------------------
// [SECTION] PlotBarGroups
//-----------------------------------------------------------------------------

template <typename T>
void PlotBarGroups(const char* const label_ids[], const T* values, int item_count, int group_count, double group_size, double shift, ImPlotBarGroupsFlags flags) {
    const bool horz = ImHasFlag(flags, ImPlotBarGroupsFlags_Horizontal);
    const bool stack = ImHasFlag(flags, ImPlotBarGroupsFlags_Stacked);
    if (stack) {
        SetupLock();
        ImPlotContext& gp = *GImPlot;
        gp.TempDouble1.resize(4*group_count);
        double* temp = gp.TempDouble1.Data;
        double* neg =      &temp[0];
        double* pos =      &temp[group_count];
        double* curr_min = &temp[group_count*2];
        double* curr_max = &temp[group_count*3];
        for (int g = 0; g < group_count*2; ++g)
            temp[g] = 0;
        if (horz) {
            for (int i = 0; i < item_count; ++i) {
                if (!IsItemHidden(label_ids[i])) {
                    for (int g = 0; g < group_count; ++g) {
                        double v = (double)values[i*group_count+g];
                        if (v > 0) {
                            curr_min[g] = pos[g];
                            curr_max[g] = curr_min[g] + v;
                            pos[g]      += v;
                        }
                        else {
                            curr_max[g] = neg[g];
                            curr_min[g] = curr_max[g] + v;
                            neg[g]      += v;
                        }
                    }
                }
                GetterXY<IndexerIdx<double>,IndexerLin> getter1(IndexerIdx<double>(curr_min,group_count),IndexerLin(1.0,shift),group_count);
                GetterXY<IndexerIdx<double>,IndexerLin> getter2(IndexerIdx<double>(curr_max,group_count),IndexerLin(1.0,shift),group_count);
                PlotBarsHEx(label_ids[i],getter1,getter2,group_size,0);
            }
        }
        else {
            for (int i = 0; i < item_count; ++i) {
                if (!IsItemHidden(label_ids[i])) {
                    for (int g = 0; g < group_count; ++g) {
                        double v = (double)values[i*group_count+g];
                        if (v > 0) {
                            curr_min[g] = pos[g];
                            curr_max[g] = curr_min[g] + v;
                            pos[g]      += v;
                        }
                        else {
                            curr_max[g] = neg[g];
                            curr_min[g] = curr_max[g] + v;
                            neg[g]      += v;
                        }
                    }
                }
                GetterXY<IndexerLin,IndexerIdx<double>> getter1(IndexerLin(1.0,shift),IndexerIdx<double>(curr_min,group_count),group_count);
                GetterXY<IndexerLin,IndexerIdx<double>> getter2(IndexerLin(1.0,shift),IndexerIdx<double>(curr_max,group_count),group_count);
                PlotBarsVEx(label_ids[i],getter1,getter2,group_size,0);
            }
        }
    }
    else {
        const double subsize = group_size / item_count;
        if (horz) {
            for (int i = 0; i < item_count; ++i) {
                const double subshift = (i+0.5)*subsize - group_size/2;
                PlotBars(label_ids[i],&values[i*group_count],group_count,subsize,subshift+shift,ImPlotBarsFlags_Horizontal);
            }
        }
        else {
            for (int i = 0; i < item_count; ++i) {
                const double subshift = (i+0.5)*subsize - group_size/2;
                PlotBars(label_ids[i],&values[i*group_count],group_count,subsize,subshift+shift);
            }
        }
    }
}

#define INSTANTIATE_MACRO(T) template IMPLOT_API void PlotBarGroups<T>(const char* const label_ids[], const T* values, int items, int groups, double width, double shift, ImPlotBarGroupsFlags flags);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotErrorBars
//-----------------------------------------------------------------------------

template <typename _GetterPos, typename _GetterNeg>
void PlotErrorBarsVEx(const char* label_id, const _GetterPos& getter_pos, const _GetterNeg& getter_neg, ImPlotErrorBarsFlags flags) {
    if (BeginItemEx(label_id, Fitter2<_GetterPos,_GetterNeg>(getter_pos, getter_neg), flags, IMPLOT_AUTO)) {
        if (getter_pos.Count <= 0 || getter_neg.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        ImDrawList& draw_list = *GetPlotDrawList();
        const ImU32 col = ImGui::GetColorU32(s.Colors[ImPlotCol_ErrorBar]);
        const bool rend_whisker  = s.ErrorBarSize > 0;
        const float half_whisker = s.ErrorBarSize * 0.5f;
        for (int i = 0; i < getter_pos.Count; ++i) {
            ImVec2 p1 = PlotToPixels(getter_neg(i),IMPLOT_AUTO,IMPLOT_AUTO);
            ImVec2 p2 = PlotToPixels(getter_pos(i),IMPLOT_AUTO,IMPLOT_AUTO);
            draw_list.AddLine(p1,p2,col, s.ErrorBarWeight);
            if (rend_whisker) {
                draw_list.AddLine(p1 - ImVec2(half_whisker, 0), p1 + ImVec2(half_whisker, 0), col, s.ErrorBarWeight);
                draw_list.AddLine(p2 - ImVec2(half_whisker, 0), p2 + ImVec2(half_whisker, 0), col, s.ErrorBarWeight);
            }
        }
        EndItem();
    }
}

template <typename _GetterPos, typename _GetterNeg>
void PlotErrorBarsHEx(const char* label_id, const _GetterPos& getter_pos, const _GetterNeg& getter_neg, ImPlotErrorBarsFlags flags) {
    if (BeginItemEx(label_id, Fitter2<_GetterPos,_GetterNeg>(getter_pos, getter_neg), flags, IMPLOT_AUTO)) {
        if (getter_pos.Count <= 0 || getter_neg.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        ImDrawList& draw_list = *GetPlotDrawList();
        const ImU32 col = ImGui::GetColorU32(s.Colors[ImPlotCol_ErrorBar]);
        const bool rend_whisker  = s.ErrorBarSize > 0;
        const float half_whisker = s.ErrorBarSize * 0.5f;
        for (int i = 0; i < getter_pos.Count; ++i) {
            ImVec2 p1 = PlotToPixels(getter_neg(i),IMPLOT_AUTO,IMPLOT_AUTO);
            ImVec2 p2 = PlotToPixels(getter_pos(i),IMPLOT_AUTO,IMPLOT_AUTO);
            draw_list.AddLine(p1, p2, col, s.ErrorBarWeight);
            if (rend_whisker) {
                draw_list.AddLine(p1 - ImVec2(0, half_whisker), p1 + ImVec2(0, half_whisker), col, s.ErrorBarWeight);
                draw_list.AddLine(p2 - ImVec2(0, half_whisker), p2 + ImVec2(0, half_whisker), col, s.ErrorBarWeight);
            }
        }
        EndItem();
    }
}

template <typename T>
void PlotErrorBars(const char* label_id, const T* xs, const T* ys, const T* err, int count, ImPlotErrorBarsFlags flags, int offset, int stride) {
    PlotErrorBars(label_id, xs, ys, err, err, count, flags, offset, stride);
}

template <typename T>
void PlotErrorBars(const char* label_id, const T* xs, const T* ys, const T* neg, const T* pos, int count, ImPlotErrorBarsFlags flags, int offset, int stride) {
    IndexerIdx<T> indexer_x(xs, count,offset,stride);
    IndexerIdx<T> indexer_y(ys, count,offset,stride);
    IndexerIdx<T> indexer_n(neg,count,offset,stride);
    IndexerIdx<T> indexer_p(pos,count,offset,stride);
    GetterError<T> getter(xs, ys, neg, pos, count, offset, stride);
    if (ImHasFlag(flags, ImPlotErrorBarsFlags_Horizontal)) {
        IndexerAdd<IndexerIdx<T>,IndexerIdx<T>> indexer_xp(indexer_x, indexer_p, 1,  1);
        IndexerAdd<IndexerIdx<T>,IndexerIdx<T>> indexer_xn(indexer_x, indexer_n, 1, -1);
        GetterXY<IndexerAdd<IndexerIdx<T>,IndexerIdx<T>>,IndexerIdx<T>> getter_p(indexer_xp, indexer_y, count);
        GetterXY<IndexerAdd<IndexerIdx<T>,IndexerIdx<T>>,IndexerIdx<T>> getter_n(indexer_xn, indexer_y, count);
        PlotErrorBarsHEx(label_id, getter_p, getter_n, flags);
    }
    else {
        IndexerAdd<IndexerIdx<T>,IndexerIdx<T>> indexer_yp(indexer_y, indexer_p, 1,  1);
        IndexerAdd<IndexerIdx<T>,IndexerIdx<T>> indexer_yn(indexer_y, indexer_n, 1, -1);
        GetterXY<IndexerIdx<T>,IndexerAdd<IndexerIdx<T>,IndexerIdx<T>>> getter_p(indexer_x, indexer_yp, count);
        GetterXY<IndexerIdx<T>,IndexerAdd<IndexerIdx<T>,IndexerIdx<T>>> getter_n(indexer_x, indexer_yn, count);
        PlotErrorBarsVEx(label_id, getter_p, getter_n, flags);
    }
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotErrorBars<T>(const char* label_id, const T* xs, const T* ys, const T* err, int count, ImPlotErrorBarsFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotErrorBars<T>(const char* label_id, const T* xs, const T* ys, const T* neg, const T* pos, int count, ImPlotErrorBarsFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotStems
//-----------------------------------------------------------------------------

template <typename _GetterM, typename _GetterB>
void PlotStemsEx(const char* label_id, const _GetterM& getter_mark, const _GetterB& getter_base, ImPlotStemsFlags flags) {
    if (BeginItemEx(label_id, Fitter2<_GetterM,_GetterB>(getter_mark,getter_base), flags, ImPlotCol_Line)) {
        if (getter_mark.Count <= 0 || getter_base.Count <= 0) {
            EndItem();
            return;
        }
        const ImPlotNextItemData& s = GetItemData();
        // render stems
        if (s.RenderLine) {
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
            RenderPrimitives2<RendererLineSegments2>(getter_mark, getter_base, col_line, s.LineWeight);
        }
        // render markers
        if (s.Marker != ImPlotMarker_None) {
            PopPlotClipRect();
            PushPlotClipRect(s.MarkerSize);
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerOutline]);
            const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_MarkerFill]);
            RenderMarkers<_GetterM>(getter_mark, s.Marker, s.MarkerSize, s.RenderMarkerFill, col_fill, s.RenderMarkerLine, col_line, s.MarkerWeight);
        }
        EndItem();
    }
}

template <typename T>
void PlotStems(const char* label_id, const T* values, int count, double ref, double scale, double start, ImPlotStemsFlags flags, int offset, int stride) {
    if (ImHasFlag(flags, ImPlotStemsFlags_Horizontal)) {
        GetterXY<IndexerIdx<T>,IndexerLin> get_mark(IndexerIdx<T>(values,count,offset,stride),IndexerLin(scale,start),count);
        GetterXY<IndexerConst,IndexerLin>  get_base(IndexerConst(ref),IndexerLin(scale,start),count);
        PlotStemsEx(label_id, get_mark, get_base, flags);
    }
    else {
        GetterXY<IndexerLin,IndexerIdx<T>> get_mark(IndexerLin(scale,start),IndexerIdx<T>(values,count,offset,stride),count);
        GetterXY<IndexerLin,IndexerConst>  get_base(IndexerLin(scale,start),IndexerConst(ref),count);
        PlotStemsEx(label_id, get_mark, get_base, flags);
    }
}

template <typename T>
void PlotStems(const char* label_id, const T* xs, const T* ys, int count, double ref, ImPlotStemsFlags flags, int offset, int stride) {
    if (ImHasFlag(flags, ImPlotStemsFlags_Horizontal)) {
        GetterXY<IndexerIdx<T>,IndexerIdx<T>> get_mark(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
        GetterXY<IndexerConst,IndexerIdx<T>>  get_base(IndexerConst(ref),IndexerIdx<T>(ys,count,offset,stride),count);
        PlotStemsEx(label_id, get_mark, get_base, flags);
    }
    else {
        GetterXY<IndexerIdx<T>,IndexerIdx<T>> get_mark(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
        GetterXY<IndexerIdx<T>,IndexerConst>  get_base(IndexerIdx<T>(xs,count,offset,stride),IndexerConst(ref),count);
        PlotStemsEx(label_id, get_mark, get_base, flags);
    }
}

#define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotStems<T>(const char* label_id, const T* values, int count, double ref, double scale, double start, ImPlotStemsFlags flags, int offset, int stride); \
    template IMPLOT_API void PlotStems<T>(const char* label_id, const T* xs, const T* ys, int count, double ref, ImPlotStemsFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO


//-----------------------------------------------------------------------------
// [SECTION] PlotInfLines
//-----------------------------------------------------------------------------

template <typename T>
void PlotInfLines(const char* label_id, const T* values, int count, ImPlotInfLinesFlags flags, int offset, int stride) {
    const ImPlotRect lims = GetPlotLimits(IMPLOT_AUTO,IMPLOT_AUTO);
    if (ImHasFlag(flags, ImPlotInfLinesFlags_Horizontal)) {
        GetterXY<IndexerConst,IndexerIdx<T>> getter_min(IndexerConst(lims.X.Min),IndexerIdx<T>(values,count,offset,stride),count);
        GetterXY<IndexerConst,IndexerIdx<T>> getter_max(IndexerConst(lims.X.Max),IndexerIdx<T>(values,count,offset,stride),count);
        if (BeginItemEx(label_id, FitterY<GetterXY<IndexerConst,IndexerIdx<T>>>(getter_min), flags, ImPlotCol_Line)) {
            if (count <= 0) {
                EndItem();
                return;
            }
            const ImPlotNextItemData& s = GetItemData();
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
            if (s.RenderLine)
                RenderPrimitives2<RendererLineSegments2>(getter_min, getter_max, col_line, s.LineWeight);
            EndItem();
        }
    }
    else {
        GetterXY<IndexerIdx<T>,IndexerConst> get_min(IndexerIdx<T>(values,count,offset,stride),IndexerConst(lims.Y.Min),count);
        GetterXY<IndexerIdx<T>,IndexerConst> get_max(IndexerIdx<T>(values,count,offset,stride),IndexerConst(lims.Y.Max),count);
        if (BeginItemEx(label_id, FitterX<GetterXY<IndexerIdx<T>,IndexerConst>>(get_min), flags, ImPlotCol_Line)) {
            if (count <= 0) {
                EndItem();
                return;
            }
            const ImPlotNextItemData& s = GetItemData();
            const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
            if (s.RenderLine)
                RenderPrimitives2<RendererLineSegments2>(get_min, get_max, col_line, s.LineWeight);
            EndItem();
        }
    }
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API void PlotInfLines<T>(const char* label_id, const T* xs, int count, ImPlotInfLinesFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotPieChart
//-----------------------------------------------------------------------------

IMPLOT_INLINE void RenderPieSlice(ImDrawList& draw_list, const ImPlotPoint& center, double radius, double a0, double a1, ImU32 col) {
    const float resolution = 50 / (2 * IM_PI);
    ImVec2 buffer[52];
    buffer[0] = PlotToPixels(center,IMPLOT_AUTO,IMPLOT_AUTO);
    int n = ImMax(3, (int)((a1 - a0) * resolution));
    double da = (a1 - a0) / (n - 1);
    int i = 0;
    for (; i < n; ++i) {
        double a = a0 + i * da;
        buffer[i + 1] = PlotToPixels(center.x + radius * cos(a), center.y + radius * sin(a),IMPLOT_AUTO,IMPLOT_AUTO);
    }
    buffer[i+1] = buffer[0];
    // fill
    draw_list.AddConvexPolyFilled(buffer, n + 1, col);
    // border (for AA)
    draw_list.AddPolyline(buffer, n + 2, col, 0, 2.0f);
}

template <typename T>
double PieChartSum(const T* values, int count, bool ignore_hidden) {
    double sum = 0;
    if (ignore_hidden) {
        ImPlotContext& gp = *GImPlot;
        ImPlotItemGroup& Items = *gp.CurrentItems;
        for (int i = 0; i < count; ++i) {
            if (i >= Items.GetItemCount())
                break;

            ImPlotItem* item = Items.GetItemByIndex(i);
            IM_ASSERT(item != nullptr);
            if (item->Show) {
                sum += (double)values[i];
            }
        }
    }
    else {
        for (int i = 0; i < count; ++i) {
            sum += (double)values[i];
        }
    }
    return sum;
}

template <typename T>
void PlotPieChartEx(const char* const label_ids[], const T* values, int count, ImPlotPoint center, double radius, double angle0, ImPlotPieChartFlags flags) {
    ImDrawList& draw_list  = *GetPlotDrawList();

    const bool ignore_hidden = ImHasFlag(flags, ImPlotPieChartFlags_IgnoreHidden);
    const double sum         = PieChartSum(values, count, ignore_hidden);
    const bool normalize     = ImHasFlag(flags, ImPlotPieChartFlags_Normalize) || sum > 1.0;

    double a0 = angle0 * 2 * IM_PI / 360.0;
    double a1 = angle0 * 2 * IM_PI / 360.0;
    ImPlotPoint Pmin = ImPlotPoint(center.x - radius, center.y - radius);
    ImPlotPoint Pmax = ImPlotPoint(center.x + radius, center.y + radius);
    for (int i = 0; i < count; ++i) {
        ImPlotItem* item = GetItem(label_ids[i]);

        const double percent = normalize ? (double)values[i] / sum : (double)values[i];
        const bool skip      = sum <= 0.0 || (ignore_hidden && item != nullptr && !item->Show);
        if (!skip)
            a1 = a0 + 2 * IM_PI * percent;

        if (BeginItemEx(label_ids[i], FitterRect(Pmin, Pmax))) {
            if (sum > 0.0) {
                ImU32 col = GetCurrentItem()->Color;
                if (percent < 0.5) {
                    RenderPieSlice(draw_list, center, radius, a0, a1, col);
                }
                else {
                    RenderPieSlice(draw_list, center, radius, a0, a0 + (a1 - a0) * 0.5, col);
                    RenderPieSlice(draw_list, center, radius, a0 + (a1 - a0) * 0.5, a1, col);
                }
            }
            EndItem();
        }
        if (!skip)
            a0 = a1;
    }
}

int PieChartFormatter(double value, char* buff, int size, void* data) {
    const char* fmt = (const char*)data;
    return snprintf(buff, size, fmt, value);
};

template <typename T>
void PlotPieChart(const char* const label_ids[], const T* values, int count, double x, double y, double radius, const char* fmt, double angle0, ImPlotPieChartFlags flags) {
    PlotPieChart<T>(label_ids, values, count, x, y, radius, PieChartFormatter, (void*)fmt, angle0, flags);
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API void PlotPieChart<T>(const char* const label_ids[], const T* values, int count, double x, double y, double radius, const char* fmt, double angle0, ImPlotPieChartFlags flags);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

template <typename T>
void PlotPieChart(const char* const label_ids[], const T* values, int count, double x, double y, double radius, ImPlotFormatter fmt, void* fmt_data, double angle0, ImPlotPieChartFlags flags) {
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != nullptr, "PlotPieChart() needs to be called between BeginPlot() and EndPlot()!");
    ImDrawList& draw_list = *GetPlotDrawList();

    const bool ignore_hidden = ImHasFlag(flags, ImPlotPieChartFlags_IgnoreHidden);
    const double sum = PieChartSum(values, count, ignore_hidden);
    const bool normalize = ImHasFlag(flags, ImPlotPieChartFlags_Normalize) || sum > 1.0;
    ImPlotPoint center(x, y);

    PushPlotClipRect();
    PlotPieChartEx(label_ids, values, count, center, radius, angle0, flags);
    if (fmt != nullptr) {
        double a0 = angle0 * 2 * IM_PI / 360.0;
        double a1 = angle0 * 2 * IM_PI / 360.0;
        char buffer[32];
        for (int i = 0; i < count; ++i) {
            ImPlotItem* item = GetItem(label_ids[i]);
            IM_ASSERT(item != nullptr);

            const double percent = normalize ? (double)values[i] / sum : (double)values[i];
            const bool skip = ignore_hidden && item != nullptr && !item->Show;

            if (!skip) {
                a1 = a0 + 2 * IM_PI * percent;
                if (item->Show) {
                    fmt((double)values[i], buffer, 32, fmt_data);
                    ImVec2 size = ImGui::CalcTextSize(buffer);
                    double angle = a0 + (a1 - a0) * 0.5;
                    ImVec2 pos = PlotToPixels(center.x + 0.5 * radius * cos(angle), center.y + 0.5 * radius * sin(angle), IMPLOT_AUTO, IMPLOT_AUTO);
                    ImU32 col = CalcTextColor(ImGui::ColorConvertU32ToFloat4(item->Color));
                    draw_list.AddText(pos - size * 0.5f, col, buffer);
                }
                a0 = a1;
            }
        }
    }
    PopPlotClipRect();
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API void PlotPieChart(const char* const label_ids[], const T* values, int count, double x, double y, double radius, ImPlotFormatter fmt, void* fmt_data, double angle0, ImPlotPieChartFlags flags);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotHeatmap
//-----------------------------------------------------------------------------

template <typename T>
struct GetterHeatmapRowMaj {
    GetterHeatmapRowMaj(const T* values, int rows, int cols, double scale_min, double scale_max, double width, double height, double xref, double yref, double ydir) :
        Values(values),
        Count(rows*cols),
        Rows(rows),
        Cols(cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        Width(width),
        Height(height),
        XRef(xref),
        YRef(yref),
        YDir(ydir),
        HalfSize(Width*0.5, Height*0.5)
    { }
    template <typename I> IMPLOT_INLINE RectC operator()(I idx) const {
        double val = (double)Values[idx];
        const int r = idx / Cols;
        const int c = idx % Cols;
        const ImPlotPoint p(XRef + HalfSize.x + c*Width, YRef + YDir * (HalfSize.y + r*Height));
        RectC rect;
        rect.Pos = p;
        rect.HalfSize = HalfSize;
        const float t = ImClamp((float)ImRemap01(val, ScaleMin, ScaleMax),0.0f,1.0f);
        ImPlotContext& gp = *GImPlot;
        rect.Color = gp.ColormapData.LerpTable(gp.Style.Colormap, t);
        return rect;
    }
    const T* const Values;
    const int Count, Rows, Cols;
    const double ScaleMin, ScaleMax, Width, Height, XRef, YRef, YDir;
    const ImPlotPoint HalfSize;
};

template <typename T>
struct GetterHeatmapColMaj {
    GetterHeatmapColMaj(const T* values, int rows, int cols, double scale_min, double scale_max, double width, double height, double xref, double yref, double ydir) :
        Values(values),
        Count(rows*cols),
        Rows(rows),
        Cols(cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        Width(width),
        Height(height),
        XRef(xref),
        YRef(yref),
        YDir(ydir),
        HalfSize(Width*0.5, Height*0.5)
    { }
    template <typename I> IMPLOT_INLINE RectC operator()(I idx) const {
        double val = (double)Values[idx];
        const int r = idx % Rows;
        const int c = idx / Rows;
        const ImPlotPoint p(XRef + HalfSize.x + c*Width, YRef + YDir * (HalfSize.y + r*Height));
        RectC rect;
        rect.Pos = p;
        rect.HalfSize = HalfSize;
        const float t = ImClamp((float)ImRemap01(val, ScaleMin, ScaleMax),0.0f,1.0f);
        ImPlotContext& gp = *GImPlot;
        rect.Color = gp.ColormapData.LerpTable(gp.Style.Colormap, t);
        return rect;
    }
    const T* const Values;
    const int Count, Rows, Cols;
    const double ScaleMin, ScaleMax, Width, Height, XRef, YRef, YDir;
    const ImPlotPoint HalfSize;
};

template <typename T>
void RenderHeatmap(ImDrawList& draw_list, const T* values, int rows, int cols, double scale_min, double scale_max, const char* fmt, const ImPlotPoint& bounds_min, const ImPlotPoint& bounds_max, bool reverse_y, bool col_maj) {
    ImPlotContext& gp = *GImPlot;
    Transformer2 transformer;
    if (scale_min == 0 && scale_max == 0) {
        T temp_min, temp_max;
        ImMinMaxArray(values,rows*cols,&temp_min,&temp_max);
        scale_min = (double)temp_min;
        scale_max = (double)temp_max;
    }
    if (scale_min == scale_max) {
        ImVec2 a = transformer(bounds_min);
        ImVec2 b = transformer(bounds_max);
        ImU32  col = GetColormapColorU32(0,gp.Style.Colormap);
        draw_list.AddRectFilled(a, b, col);
        return;
    }
    const double yref = reverse_y ? bounds_max.y : bounds_min.y;
    const double ydir = reverse_y ? -1 : 1;
    if (col_maj) {
        GetterHeatmapColMaj<T> getter(values, rows, cols, scale_min, scale_max, (bounds_max.x - bounds_min.x) / cols, (bounds_max.y - bounds_min.y) / rows, bounds_min.x, yref, ydir);
        RenderPrimitives1<RendererRectC>(getter);
    }
    else {
        GetterHeatmapRowMaj<T> getter(values, rows, cols, scale_min, scale_max, (bounds_max.x - bounds_min.x) / cols, (bounds_max.y - bounds_min.y) / rows, bounds_min.x, yref, ydir);
        RenderPrimitives1<RendererRectC>(getter);
    }
    // labels
    if (fmt != nullptr) {
        const double w = (bounds_max.x - bounds_min.x) / cols;
        const double h = (bounds_max.y - bounds_min.y) / rows;
        const ImPlotPoint half_size(w*0.5,h*0.5);
        int i = 0;
        if (col_maj) {
            for (int c = 0; c < cols; ++c) {
                for (int r = 0; r < rows; ++r) {
                    ImPlotPoint p;
                    p.x = bounds_min.x + 0.5*w + c*w;
                    p.y = yref + ydir * (0.5*h + r*h);
                    ImVec2 px = transformer(p);
                    char buff[32];
                    ImFormatString(buff, 32, fmt, values[i]);
                    ImVec2 size = ImGui::CalcTextSize(buff);
                    double t = ImClamp(ImRemap01((double)values[i], scale_min, scale_max),0.0,1.0);
                    ImVec4 color = SampleColormap((float)t);
                    ImU32 col = CalcTextColor(color);
                    draw_list.AddText(px - size * 0.5f, col, buff);
                    i++;
                }
            }
        }
        else {
            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    ImPlotPoint p;
                    p.x = bounds_min.x + 0.5*w + c*w;
                    p.y = yref + ydir * (0.5*h + r*h);
                    ImVec2 px = transformer(p);
                    char buff[32];
                    ImFormatString(buff, 32, fmt, values[i]);
                    ImVec2 size = ImGui::CalcTextSize(buff);
                    double t = ImClamp(ImRemap01((double)values[i], scale_min, scale_max),0.0,1.0);
                    ImVec4 color = SampleColormap((float)t);
                    ImU32 col = CalcTextColor(color);
                    draw_list.AddText(px - size * 0.5f, col, buff);
                    i++;
                }
            }
        }
    }
}

template <typename T>
void PlotHeatmap(const char* label_id, const T* values, int rows, int cols, double scale_min, double scale_max, const char* fmt, const ImPlotPoint& bounds_min, const ImPlotPoint& bounds_max, ImPlotHeatmapFlags flags) {
    if (BeginItemEx(label_id, FitterRect(bounds_min, bounds_max))) {
        if (rows <= 0 || cols <= 0) {
            EndItem();
            return;
        }
        ImDrawList& draw_list = *GetPlotDrawList();
        const bool col_maj = ImHasFlag(flags, ImPlotHeatmapFlags_ColMajor);
        RenderHeatmap(draw_list, values, rows, cols, scale_min, scale_max, fmt, bounds_min, bounds_max, true, col_maj);
        EndItem();
    }
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API void PlotHeatmap<T>(const char* label_id, const T* values, int rows, int cols, double scale_min, double scale_max, const char* fmt, const ImPlotPoint& bounds_min, const ImPlotPoint& bounds_max, ImPlotHeatmapFlags flags);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotHistogram
//-----------------------------------------------------------------------------

template <typename T>
double PlotHistogram(const char* label_id, const T* values, int count, int bins, double bar_scale, ImPlotRange range, ImPlotHistogramFlags flags) {

    const bool cumulative = ImHasFlag(flags, ImPlotHistogramFlags_Cumulative);
    const bool density    = ImHasFlag(flags, ImPlotHistogramFlags_Density);
    const bool outliers   = !ImHasFlag(flags, ImPlotHistogramFlags_NoOutliers);

    if (count <= 0 || bins == 0)
        return 0;

    if (range.Min == 0 && range.Max == 0) {
        T Min, Max;
        ImMinMaxArray(values, count, &Min, &Max);
        range.Min = (double)Min;
        range.Max = (double)Max;
    }

    double width;
    if (bins < 0)
        CalculateBins(values, count, bins, range, bins, width);
    else
        width = range.Size() / bins;

    ImPlotContext& gp = *GImPlot;
    ImVector<double>& bin_centers = gp.TempDouble1;
    ImVector<double>& bin_counts  = gp.TempDouble2;
    bin_centers.resize(bins);
    bin_counts.resize(bins);
    int below = 0;

    for (int b = 0; b < bins; ++b) {
        bin_centers[b] = range.Min + b * width + width * 0.5;
        bin_counts[b] = 0;
    }
    int counted = 0;
    double max_count = 0;
    for (int i = 0; i < count; ++i) {
        double val = (double)values[i];
        if (range.Contains(val)) {
            const int b = ImClamp((int)((val - range.Min) / width), 0, bins - 1);
            bin_counts[b] += 1.0;
            if (bin_counts[b] > max_count)
                max_count = bin_counts[b];
            counted++;
        }
        else if (val < range.Min) {
            below++;
        }
    }
    if (cumulative && density) {
        if (outliers)
            bin_counts[0] += below;
        for (int b = 1; b < bins; ++b)
            bin_counts[b] += bin_counts[b-1];
        double scale = 1.0 / (outliers ? count : counted);
        for (int b = 0; b < bins; ++b)
            bin_counts[b] *= scale;
        max_count = bin_counts[bins-1];
    }
    else if (cumulative) {
        if (outliers)
            bin_counts[0] += below;
        for (int b = 1; b < bins; ++b)
            bin_counts[b] += bin_counts[b-1];
        max_count = bin_counts[bins-1];
    }
    else if (density) {
        double scale = 1.0 / ((outliers ? count : counted) * width);
        for (int b = 0; b < bins; ++b)
            bin_counts[b] *= scale;
        max_count *= scale;
    }
    if (ImHasFlag(flags, ImPlotHistogramFlags_Horizontal))
        PlotBars(label_id, &bin_counts.Data[0], &bin_centers.Data[0], bins, bar_scale*width, ImPlotBarsFlags_Horizontal);
    else
        PlotBars(label_id, &bin_centers.Data[0], &bin_counts.Data[0], bins, bar_scale*width);
    return max_count;
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API double PlotHistogram<T>(const char* label_id, const T* values, int count, int bins, double bar_scale, ImPlotRange range, ImPlotHistogramFlags flags);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotHistogram2D
//-----------------------------------------------------------------------------

template <typename T>
double PlotHistogram2D(const char* label_id, const T* xs, const T* ys, int count, int x_bins, int y_bins, ImPlotRect range, ImPlotHistogramFlags flags) {

    // const bool cumulative = ImHasFlag(flags, ImPlotHistogramFlags_Cumulative); NOT SUPPORTED
    const bool density  = ImHasFlag(flags, ImPlotHistogramFlags_Density);
    const bool outliers = !ImHasFlag(flags, ImPlotHistogramFlags_NoOutliers);
    const bool col_maj  = ImHasFlag(flags, ImPlotHistogramFlags_ColMajor);

    if (count <= 0 || x_bins == 0 || y_bins == 0)
        return 0;

    if (range.X.Min == 0 && range.X.Max == 0) {
        T Min, Max;
        ImMinMaxArray(xs, count, &Min, &Max);
        range.X.Min = (double)Min;
        range.X.Max = (double)Max;
    }
    if (range.Y.Min == 0 && range.Y.Max == 0) {
        T Min, Max;
        ImMinMaxArray(ys, count, &Min, &Max);
        range.Y.Min = (double)Min;
        range.Y.Max = (double)Max;
    }

    double width, height;
    if (x_bins < 0)
        CalculateBins(xs, count, x_bins, range.X, x_bins, width);
    else
        width = range.X.Size() / x_bins;
    if (y_bins < 0)
        CalculateBins(ys, count, y_bins, range.Y, y_bins, height);
    else
        height = range.Y.Size() / y_bins;

    const int bins = x_bins * y_bins;

    ImPlotContext& gp = *GImPlot;
    ImVector<double>& bin_counts = gp.TempDouble1;
    bin_counts.resize(bins);

    for (int b = 0; b < bins; ++b)
        bin_counts[b] = 0;

    int counted = 0;
    double max_count = 0;
    for (int i = 0; i < count; ++i) {
        if (range.Contains((double)xs[i], (double)ys[i])) {
            const int xb = ImClamp( (int)((double)(xs[i] - range.X.Min) / width)  , 0, x_bins - 1);
            const int yb = ImClamp( (int)((double)(ys[i] - range.Y.Min) / height) , 0, y_bins - 1);
            const int b  = yb * x_bins + xb;
            bin_counts[b] += 1.0;
            if (bin_counts[b] > max_count)
                max_count = bin_counts[b];
            counted++;
        }
    }
    if (density) {
        double scale = 1.0 / ((outliers ? count : counted) * width * height);
        for (int b = 0; b < bins; ++b)
            bin_counts[b] *= scale;
        max_count *= scale;
    }

    if (BeginItemEx(label_id, FitterRect(range))) {
        if (y_bins <= 0 || x_bins <= 0) {
            EndItem();
            return max_count;
        }
        ImDrawList& draw_list = *GetPlotDrawList();
        RenderHeatmap(draw_list, &bin_counts.Data[0], y_bins, x_bins, 0, max_count, nullptr, range.Min(), range.Max(), false, col_maj);
        EndItem();
    }
    return max_count;
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API double PlotHistogram2D<T>(const char* label_id,   const T*   xs, const T*   ys, int count, int x_bins, int y_bins, ImPlotRect range, ImPlotHistogramFlags flags);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

//-----------------------------------------------------------------------------
// [SECTION] PlotDigital
//-----------------------------------------------------------------------------

// TODO: Make this behave like all the other plot types (.e. not fixed in y axis)

template <typename Getter>
void PlotDigitalEx(const char* label_id, Getter getter, ImPlotDigitalFlags flags) {
    if (BeginItem(label_id, flags, ImPlotCol_Fill)) {
        ImPlotContext& gp = *GImPlot;
        ImDrawList& draw_list = *GetPlotDrawList();
        const ImPlotNextItemData& s = GetItemData();
        if (getter.Count > 1 && s.RenderFill) {
            ImPlotPlot& plot   = *gp.CurrentPlot;
            ImPlotAxis& x_axis = plot.Axes[plot.CurrentX];
            ImPlotAxis& y_axis = plot.Axes[plot.CurrentY];

            int pixYMax = 0;
            ImPlotPoint itemData1 = getter(0);
            for (int i = 0; i < getter.Count; ++i) {
                ImPlotPoint itemData2 = getter(i);
                if (ImNanOrInf(itemData1.y)) {
                    itemData1 = itemData2;
                    continue;
                }
                if (ImNanOrInf(itemData2.y)) itemData2.y = ImConstrainNan(ImConstrainInf(itemData2.y));
                int pixY_0 = (int)(s.LineWeight);
                itemData1.y = ImMax(0.0, itemData1.y);
                float pixY_1_float = s.DigitalBitHeight * (float)itemData1.y;
                int pixY_1 = (int)(pixY_1_float); //allow only positive values
                int pixY_chPosOffset = (int)(ImMax(s.DigitalBitHeight, pixY_1_float) + s.DigitalBitGap);
                pixYMax = ImMax(pixYMax, pixY_chPosOffset);
                ImVec2 pMin = PlotToPixels(itemData1,IMPLOT_AUTO,IMPLOT_AUTO);
                ImVec2 pMax = PlotToPixels(itemData2,IMPLOT_AUTO,IMPLOT_AUTO);
                int pixY_Offset = 0; //20 pixel from bottom due to mouse cursor label
                pMin.y = (y_axis.PixelMin) + ((-gp.DigitalPlotOffset)                   - pixY_Offset);
                pMax.y = (y_axis.PixelMin) + ((-gp.DigitalPlotOffset) - pixY_0 - pixY_1 - pixY_Offset);
                //plot only one rectangle for same digital state
                while (((i+2) < getter.Count) && (itemData1.y == itemData2.y)) {
                    const int in = (i + 1);
                    itemData2 = getter(in);
                    if (ImNanOrInf(itemData2.y)) break;
                    pMax.x = PlotToPixels(itemData2,IMPLOT_AUTO,IMPLOT_AUTO).x;
                    i++;
                }
                //do not extend plot outside plot range
                if (pMin.x < x_axis.PixelMin) pMin.x = x_axis.PixelMin;
                if (pMax.x < x_axis.PixelMin) pMax.x = x_axis.PixelMin;
                if (pMin.x > x_axis.PixelMax) pMin.x = x_axis.PixelMax - 1; //fix issue related to https://github.com/ocornut/imgui/issues/3976
                if (pMax.x > x_axis.PixelMax) pMax.x = x_axis.PixelMax - 1; //fix issue related to https://github.com/ocornut/imgui/issues/3976
                //plot a rectangle that extends up to x2 with y1 height
                if ((pMax.x > pMin.x) && (gp.CurrentPlot->PlotRect.Contains(pMin) || gp.CurrentPlot->PlotRect.Contains(pMax))) {
                    // ImVec4 colAlpha = item->Color;
                    // colAlpha.w = item->Highlight ? 1.0f : 0.9f;
                    draw_list.AddRectFilled(pMin, pMax, ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]));
                }
                itemData1 = itemData2;
            }
            gp.DigitalPlotItemCnt++;
            gp.DigitalPlotOffset += pixYMax;
        }
        EndItem();
    }
}


template <typename T>
void PlotDigital(const char* label_id, const T* xs, const T* ys, int count, ImPlotDigitalFlags flags, int offset, int stride) {
    GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter(IndexerIdx<T>(xs,count,offset,stride),IndexerIdx<T>(ys,count,offset,stride),count);
    return PlotDigitalEx(label_id, getter, flags);
}
#define INSTANTIATE_MACRO(T) template IMPLOT_API void PlotDigital<T>(const char* label_id, const T* xs, const T* ys, int count, ImPlotDigitalFlags flags, int offset, int stride);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO

// custom
void PlotDigitalG(const char* label_id, ImPlotGetter getter_func, void* data, int count, ImPlotDigitalFlags flags) {
    GetterFuncPtr getter(getter_func,data,count);
    return PlotDigitalEx(label_id, getter, flags);
}

//-----------------------------------------------------------------------------
// [SECTION] PlotImage
//-----------------------------------------------------------------------------

void PlotImage(const char* label_id, ImTextureID user_texture_id, const ImPlotPoint& bmin, const ImPlotPoint& bmax, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, ImPlotImageFlags) {
    if (BeginItemEx(label_id, FitterRect(bmin,bmax))) {
        ImU32 tint_col32 = ImGui::ColorConvertFloat4ToU32(tint_col);
        GetCurrentItem()->Color = tint_col32;
        ImDrawList& draw_list = *GetPlotDrawList();
        ImVec2 p1 = PlotToPixels(bmin.x, bmax.y,IMPLOT_AUTO,IMPLOT_AUTO);
        ImVec2 p2 = PlotToPixels(bmax.x, bmin.y,IMPLOT_AUTO,IMPLOT_AUTO);
        PushPlotClipRect();
        draw_list.AddImage(user_texture_id, p1, p2, uv0, uv1, tint_col32);
        PopPlotClipRect();
        EndItem();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] PlotText
//-----------------------------------------------------------------------------

void PlotText(const char* text, double x, double y, const ImVec2& pixel_offset, ImPlotTextFlags flags) {
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != nullptr, "PlotText() needs to be called between BeginPlot() and EndPlot()!");
    SetupLock();
    ImDrawList & draw_list = *GetPlotDrawList();
    PushPlotClipRect();
    ImU32 colTxt = GetStyleColorU32(ImPlotCol_InlayText);
    if (ImHasFlag(flags,ImPlotTextFlags_Vertical)) {
        ImVec2 siz = CalcTextSizeVertical(text) * 0.5f;
        ImVec2 ctr = siz * 0.5f;
        ImVec2 pos = PlotToPixels(ImPlotPoint(x,y),IMPLOT_AUTO,IMPLOT_AUTO) + ImVec2(-ctr.x, ctr.y) + pixel_offset;
        if (FitThisFrame() && !ImHasFlag(flags, ImPlotItemFlags_NoFit)) {
            FitPoint(PixelsToPlot(pos));
            FitPoint(PixelsToPlot(pos.x + siz.x, pos.y - siz.y));
        }
        AddTextVertical(&draw_list, pos, colTxt, text);
    }
    else {
        ImVec2 siz = ImGui::CalcTextSize(text);
        ImVec2 pos = PlotToPixels(ImPlotPoint(x,y),IMPLOT_AUTO,IMPLOT_AUTO) - siz * 0.5f + pixel_offset;
        if (FitThisFrame() && !ImHasFlag(flags, ImPlotItemFlags_NoFit)) {
            FitPoint(PixelsToPlot(pos));
            FitPoint(PixelsToPlot(pos+siz));
        }
        draw_list.AddText(pos, colTxt, text);
    }
    PopPlotClipRect();
}

//-----------------------------------------------------------------------------
// [SECTION] PlotDummy
//-----------------------------------------------------------------------------

void PlotDummy(const char* label_id, ImPlotDummyFlags flags) {
    if (BeginItem(label_id, flags, ImPlotCol_Line))
        EndItem();
}

} // namespace ImPlot



extern "C"
void igShowImPlotDemoWindow(bool* p_open) {
    ImPlot::ShowDemoWindow(p_open);
}

