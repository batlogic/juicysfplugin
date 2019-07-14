//
//  Model.cpp
//  Lazarus
//
//  Created by Alex Birch on 01/09/2017.
//
//

#include "TableComponent.h"
#include "Util.h"

using namespace std;
using namespace Util;

//==============================================================================
/**
    This class shows how to implement a TableListBoxModel to show in a TableListBox.
*/
TableComponent::TableComponent(
    AudioProcessorValueTreeState& valueTreeState
    // const vector<string> &columns,
//    const vector<TableRow> &rows,
    // const function<void (int)> &onRowSelected,
    // const function<int (const vector<string>&)> &rowToIDMapper,
    // int initiallySelectedRow
)
: valueTreeState{valueTreeState}
, font{14.0f}
//, columns{columns}
//, rows{rows}
// , onRowSelected{onRowSelected}
// rowToIDMapper(rowToIDMapper)
{
    // Create our table component and add it to this component..
    addAndMakeVisible (table);
    table.setModel (this);

    // give it a border
    table.setColour (ListBox::outlineColourId, Colours::grey);
    table.setOutlineThickness (1);

    int columnIx = 1;

    // Add some columns to the table header, based on the column list in our database..
    // for (auto &column : columns) // access by reference to avoid copying
    // {
    //     const int colWidth{ columnIx == 1 ? 30 : 200 };
    //     table.getHeader().addColumn (
    //             String(column),
    //             columnIx++,
    //             colWidth, // column width
    //             30, // min width
    //             400, // max width
    //             TableHeaderComponent::defaultFlags
    //     );
    // }
    table.getHeader().addColumn (
            String("#"),
            columnIx++,
            30, // column width
            30, // min width
            400, // max width
            TableHeaderComponent::defaultFlags
    );
    table.getHeader().addColumn (
            String("Name"),
            columnIx++,
            200, // column width
            30, // min width
            400, // max width
            TableHeaderComponent::defaultFlags
    );

    table.setWantsKeyboardFocus(false);

    // table.selectRow();
    ValueTree presets{valueTreeState.state.getChildWithName("presets")};
    loadModelFrom(presets);
    // selectCurrentPreset();

    // we could now change some initial settings..
    table.getHeader().setSortColumnId(1, false); // sort ascending by ID column
//    table.getHeader().setColumnVisible (7, false); // hide the "length" column until the user shows it

    // un-comment this line to have a go of stretch-to-fit mode
    // table.getHeader().setStretchToFitActive (true);

//    table.setMultipleSelectionEnabled (false);
    valueTreeState.state.addListener(this);
}

TableComponent::~TableComponent() {
    valueTreeState.state.removeListener(this);
}

void TableComponent::loadModelFrom(ValueTree& presets) {
    rows.clear();
    int numChildren{presets.getNumChildren()};
    for(int i{0}; i<numChildren; i++) {
        ValueTree child{presets.getChild(i)};
        int num{child.getProperty("num")};
        String name{child.getProperty("name")};
        rows.emplace_back(num, name);
    }
    table.deselectAllRows();
    table.updateContent();
    table.getHeader().setSortColumnId(0, true);
    selectCurrentPreset();
    table.repaint();
}

void TableComponent::parameterChanged(const String& parameterID, float newValue) {
    // valueTreeState.getParameter
    if (parameterID == "preset") {
        selectCurrentPreset();
        // RangedAudioParameter *param {valueTreeState.getParameter("preset")};
        // jassert(dynamic_cast<AudioParameterInt*> (param) != nullptr);
        // AudioParameterInt* castParam {dynamic_cast<AudioParameterInt*> (param)};
        // int value{castParam->get()};
    }
}

void TableComponent::valueTreePropertyChanged(
    ValueTree& treeWhosePropertyHasChanged,
    const Identifier& property) {
    if (treeWhosePropertyHasChanged.getType() == StringRef("presets")) {
        if (property == StringRef("synthetic")) {
            loadModelFrom(treeWhosePropertyHasChanged);
        }
    }
}

// void TableComponent::valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) {
//     if (treeWhoseParentHasChanged.getType() == StringRef("presets")) {
//         loadModelFrom(treeWhoseParentHasChanged);
//     }
// }

// void TableComponent::valueTreePropertyChanged(
//     ValueTree& treeWhosePropertyHasChanged,
//     const Identifier& property) {
//     DEBUG_PRINT(treeWhosePropertyHasChanged.getType().toString());
// }
// void TableComponent::valueTreeChildAdded(
//     ValueTree& parentTree,
//     ValueTree& childWhichHasBeenAdded) {
//     DEBUG_PRINT(parentTree.getType().toString());
// }
// void TableComponent::valueTreeChildRemoved(
//     ValueTree& parentTree,
//     ValueTree& childWhichHasBeenRemoved,
//     int indexFromWhichChildWasRemoved) {
//     DEBUG_PRINT(parentTree.getType().toString());
// }
// void TableComponent::valueTreeChildOrderChanged(
//     ValueTree& parentTreeWhoseChildrenHaveMoved,
//     int oldIndex,
//     int newIndex) {
//     DEBUG_PRINT(parentTreeWhoseChildrenHaveMoved.getType().toString());
// }
// void TableComponent::valueTreeParentChanged(
//     ValueTree& treeWhoseParentHasChanged) {
//     DEBUG_PRINT(treeWhoseParentHasChanged.getType().toString());
// }
// void TableComponent::valueTreeRedirected(
//     ValueTree& treeWhichHasBeenChanged) {
//     DEBUG_PRINT(treeWhichHasBeenChanged.getType().toString());
// }

// void TableComponent::setRows(const vector<vector<string>>& rows, int initiallySelectedRow) {
//     this->rows = rows;
//     table.deselectAllRows();
//     table.updateContent();
//     table.getHeader().setSortColumnId(0, true);
//     table.selectRow(initiallySelectedRow);
//     table.repaint();
// }

// This is overloaded from TableListBoxModel, and must return the total number of rows in our table
int TableComponent::getNumRows()
{
    return static_cast<int>(rows.size());
}

// This is overloaded from TableListBoxModel, and should fill in the background of the whole row
void TableComponent::paintRowBackground (
        Graphics& g,
        int rowNumber,
        int /*width*/,
        int /*height*/,
        bool rowIsSelected
) {
    const Colour alternateColour (getLookAndFeel().findColour (ListBox::backgroundColourId)
            .interpolatedWith (getLookAndFeel().findColour (ListBox::textColourId), 0.03f));
    if (rowIsSelected)
        g.fillAll (Colours::lightblue);
    else if (rowNumber % 2)
        g.fillAll (alternateColour);
}

String TableRow::getStringContents(int columnId) {
    if (columnId <= 1) {
        return String(preset);
    }
    return name;
}

// This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
// components.
void TableComponent::paintCell (
        Graphics& g,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool /*rowIsSelected*/
) {
    g.setColour (getLookAndFeel().findColour (ListBox::textColourId));
    g.setFont (font);

    TableRow& row{rows[rowNumber]};
    String text{row.getStringContents(columnId)};
    g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);

    g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
    g.fillRect (width - 1, 0, 1, height);
}

// This is overloaded from TableListBoxModel, and tells us that the user has clicked a table header
// to change the sort order.
void TableComponent::sortOrderChanged (
        int newSortColumnId,
        bool isForwards
) {
    if (newSortColumnId != 0) {
        // int selectedRowIx = table.getSelectedRow();
        // TableRow* selectedRow;
        // if (selectedRowIx >= 0) {
        //     selectedRow = &rows[selectedRowIx];
        // }

        TableComponent::DataSorter sorter (newSortColumnId, isForwards);
        sort(rows.begin(), rows.end(), sorter);

        table.updateContent();
        selectCurrentPreset();

        // if (selectedRowIx >= 0) {
        //     for (auto it = rows.begin(); it != rows.end(); ++it) {
        //         if(it->preset == value) {
        //             int index {static_cast<int>(std::distance(rows.begin(), it))};
        //             table.selectRow(index);
        //             break;
        //         }
        //     }
        // }
    }
}

void TableComponent::selectCurrentPreset() {
    table.deselectAllRows();
    RangedAudioParameter *param {valueTreeState.getParameter("preset")};
    jassert(dynamic_cast<AudioParameterInt*> (param) != nullptr);
    AudioParameterInt* castParam {dynamic_cast<AudioParameterInt*> (param)};
    int value{castParam->get()};

    for (auto it = rows.begin(); it != rows.end(); ++it) {
        if(it->preset == value) {
            int index {static_cast<int>(std::distance(rows.begin(), it))};
            table.selectRow(index);
            break;
        }
    }
}

// This is overloaded from TableListBoxModel, and should choose the best width for the specified
// column.
int TableComponent::getColumnAutoSizeWidth (int columnId) {
    if (columnId == 1)
        return 30;

    
    int widest = 32;

    // find the widest bit of text in this column..
    for (int i{getNumRows()}; --i >= 0;) {
        TableRow& row{rows[i]};
        String text{row.getStringContents(columnId)};
        widest = jmax (widest, font.getStringWidth (text));
    }

    return widest + 8;
}

//==============================================================================
void TableComponent::resized() {
    // position our table with a gap around its edge
    table.setBoundsInset (BorderSize<int> (7));
}

//==============================================================================
// A comparator used to sort our data when the user clicks a column header

TableComponent::DataSorter::DataSorter (
        int columnByWhichToSort,
        bool forwards
)
: columnByWhichToSort (columnByWhichToSort)
, direction (forwards ? 1 : -1)
{}

bool TableComponent::DataSorter::operator ()(
        TableRow first,
        TableRow second
) {
    int result;
    if (columnByWhichToSort <= 1) {
        result = compare(first.preset, second.preset);
    } else {
        result = first.name
            .compareNatural (second.name);
        if (result == 0) {
            result = compare(first.preset, second.preset);
        }
    }

    result *= direction;

    return result > 0;
}

void TableComponent::selectedRowsChanged (int row) {
    if (row < 0) {
        return;
    }
    // onRowSelected(rowToIDMapper(rows[row]));
    // onRowSelected(stoi(rows[row][0]));
    int newPreset{rows[row].preset};
    RangedAudioParameter *param {valueTreeState.getParameter("preset")};
    jassert(dynamic_cast<AudioParameterInt*> (param) != nullptr);
    AudioParameterInt* castParam {dynamic_cast<AudioParameterInt*> (param)};
    *castParam = newPreset;
}

bool TableComponent::keyPressed(const KeyPress &key) {
    return table.keyPressed(key);
}

TableRow::TableRow(
    int preset,
    String name
)
: preset{preset}
, name{name}
{}
