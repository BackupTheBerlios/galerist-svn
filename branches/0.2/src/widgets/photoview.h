/***************************************************************************
 *   Copyright (C) 2006 by Gregor KaliÅ¡nik                                 *
 *   Copyright (C) 2006 by Jernej Kos                                      *
 *   Copyright (C) 2006 by Unimatrix-One                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef GWIDGETSPHOTOVIEW_H
#define GWIDGETSPHOTOVIEW_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QList>

#include <QtGui/QGraphicsView>

class QTimeLine;
class QRubberBand;

namespace GWidgets
{

class SearchBar;

namespace GPhotoWidgets
{
class PhotoItem;
class PhotoLoading;
}

/**
 * This class is a custom listview-like widget implemented using
 * the QGraphicsView class.
 * @short A view for showing photos, using a model.
 *
 * @author Jernej Kos <kostko@unimatrix-one.org>
 * @author Gregor KaliÅ¡nik <gregor@unimatrix-one.org>
 */
class PhotoView : public QGraphicsView
{
    Q_OBJECT
    friend class GPhotoWidgets::PhotoItem;
  signals:
    /**
     * Signal that reports that a item has been selected.
     *
     * @param selected If an item has been selected.
     */
    void signalSelected(bool selected);
    /**
     * Signal that signals that only one item has been selected.
     *
     * @param oneSelected If only one item has been selected.
     */
    void signalOneSelected(bool oneSelected);
    /**
     * Signal that reports the view has gone into editing mode. Or not.
     *
     * @param edit We are going into editing mode or out of it.
     */
    void signalEditMode(bool edit);
    /**
     * Emitted when search returns an empty result.
     */
    void noPhotosFound();
    /**
     * Emitted when moving through images at edit mode.
     */
    void photoEditSelectionChanged(int newLocation, int totalImages);
    /**
     * Emitted at mouseReleaseEvent and when m_needRuberBand is set to true. You have to manually revert every change that you've
     * done to the viewport (such as changing mouse cursors etc.).
     *
     * @param area Area of selection.
     */
    void areaSelected(const QRect &area);
    /**
     * Emitted when image has been switched through next or previous button in edit mode.
     *
     * @param size Size of the switched image.
     */
    void imageSwitched(const QSize &size);

  public:
    /**
     * Class constructor.
     *
     * @param parent Optional parent widget
     */
    PhotoView(QWidget *parent = 0);

    /**
     * Set item spacing. If not set, value of 10 is used.
     *
     * @param spacing Item spacing (horizontal and vertical) in pixels
     */
    void setSpacing(int spacing);

    /**
     * Set a model for this view.
     *
     * @param model The actual model
     */
    void setModel(QAbstractItemModel *model);

    /**
     * Get root's index.
     *
     * @return Returns a QModelIndex of the defined root index.
     */
    QModelIndex rootIndex();

    /**
     * Converts an index into an item.
     *
     * @param index Index we want to convert.
     *
     * @return Reference to the item.
     */
    GPhotoWidgets::PhotoItem *itemForIndex(const QModelIndex &index);

    /**
     * Method to get reference to the model that PhotoView is using.
     *
     * @return Reference to the model.
     */
    QAbstractItemModel *model();

    /**
     * Show the loading image or hide it.
     *
     * @param show Show it or hide it.
     * @param text Text to show. It's unusable when hiding loading item.
     */
    void showLoading(bool show, const QString &text = QString());

    /**
     * Wrapper method for retrieving reference to a selected item. If none or more are selected, returns null.
     *
     * @return Reference to the selected item.
     * @return @c null More than one or none items selected.
     */
    GPhotoWidgets::PhotoItem *getSelectedPhoto();

    /**
     * Default destructor.
     */
    ~PhotoView();

  public slots:
    /**
     * Slot for setting root index.
     *
     * @param index The new root index.
     */
    void setRootIndex(const QModelIndex &index);

    /**
     * Wrapper method for enabling edit mode.
     */
    void slotEdit();
    /**
     * Initiates the renaming sequence of the selected item.
     */
    void slotRename();
    /**
     * Initiates the describing sequence of the selected item.
     */
    void slotDescribe();

    /**
     * Slot for selecting all items.
     */
    void slotSelectAll();
    /**
     * Slot for deselecting all items.
     */
    void slotDeselectAll();
    /**
     * Slot for inverting selection.
     */
    void slotInvertSelection();

    /**
     * Retakes the focus.
     */
    void slotRetakeFocus();

    /**
     * Sets the filter.
     */
    void setFilter(const QString &filter);

    /**
     * Initiates the operation.
     */
    void initiateOperation(int operation);

    /**
     * Cancels the operation.
     */
    void cancelOperation(int operation);

    /**
     * Saves the operation results.
     */
    void saveOperation(int operation, const QMap<int, QVariant> &params);

    /**
     * Creates the preview of the operation.
     */
    void previewOperation(int operation, const QMap<int, QVariant> &params);

    /**
     * Rotates the current image (edit mode only) clock-wise.
     */
    void rotateSelectedImageCW();
    /**
     * Rotates the current image (edit mode only) counter clock-wise.
     */
    void rotateSelectedImageCCW();

    /**
     * Slot for switching to the next photo.
     */
    void slotNextPhoto();
    /**
     * Slot for moving to the previous photo.
     */
    void slotPreviousPhoto();
    /**
     * Slot for zooming in.
     */
    void slotZoomInPhoto();
    /**
     * Slot for zooming out.
     */
    void slotZoomOutPhoto();
    /**
     * Slot for zooming to actual size.
     */
    void slotZoomActualPhoto();
    /**
     * Slot for zooming to fit screen.
     */
    void slotZoomScreenPhoto();
    /**
     * Slot for input zoom.
     */
    void slotZoomInputPhoto();
    /**
     * Slot for editing the photograph.
     */
    void slotEditPhoto();
    /**
     * Slot for exiting the editing mode.
     */
    void slotExitEdit();

  protected:
    /**
     * Rearrange the items to new positions.
     *
     * @param update Should the view be updated
     */
    int rearrangeItems(bool update = true);

    /**
     * Method for read model and repopulate the view.
     */
    void readModel();

    /**
     * Overloaded method that defines what to do on resizing.
     *
     * @param event Reference to the event itself.
     */
    void resizeEvent(QResizeEvent *event);

    /**
     * Overloaded method for defining mouse press event.
     * It's used for selection zoom.
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * Overloaded method for defining mouse move event.
     * It's used for selection zoom.
     */
    void mouseMoveEvent(QMouseEvent *event);

    /**
     * Overloaded method for defining mouse release event.
     * It's used for selection zoom.
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
    * Defines what menu to show at context menu event.
    *
    * @param event Event itself.
    */
    void contextMenuEvent(QContextMenuEvent *event);

    /**
     * Method that allows Drag in images.
     *
     * @param event The event.
     */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * Method that allows Drag in images.
     *
     * @param event The event.
     */
    void dragMoveEvent(QDragMoveEvent *event);
    /**
     * Method that allows Drag in images.
     *
     * @param event The event.
     */
    void dragLeaveEvent(QDragLeaveEvent *event);
    /**
     * The implemetation of drop in action. It parses the links and passes it to ImageModel and reports to MainWindow.
     *
     * @param event Event itself.
     */
    void dropEvent(QDropEvent *event);

    /**
     * Reimplemented method.
     * Implemetation of navigation with keys.
     */
    void keyPressEvent(QKeyEvent *event);

  private:
    int m_spacing;
    QPointF m_zero;
    int m_timerId;
    bool m_editMode;

    GPhotoWidgets::PhotoItem *m_currentEdited;

    QAbstractItemModel *m_model;
    QModelIndex m_rootIndex;
    QHash<QModelIndex, GPhotoWidgets::PhotoItem*> m_itemHash;
    QVector<GPhotoWidgets::PhotoItem*> m_itemVector;

    QList<GPhotoWidgets::PhotoItem*> m_removeList;

    GPhotoWidgets::PhotoLoading *m_loading;

    int m_oldHorizontalScrollMaximum;
    int m_oldVerticalScrollMaximum;

    QList<QGraphicsItem*> m_oldSelectedItems;

    QPoint m_rubberStartPos;
    int m_rubberScrollValue;
    QRubberBand *m_rubberBand;
    bool m_needRubberBand;

    QString m_filter;

    /**
     * Update and start the updating timer.
     */
    void updateScene();
    /**
     * Updates the scroll bars (resizes the scene rectangle).
     */
    void updateScrollBars();

    /**
     * For going into editing mode. Or out of it.
     *
     * @param editMode Set or unset editing mode.
     * @param selectedItem Reference to the selected item (which item did we double clicked?).
     */
    void setEditMode(bool editMode, GPhotoWidgets::PhotoItem *selectedItem = 0);

  private slots:
    /**
    * Slot to execute when scene changes.
     */
    void slotSceneChanged();
    /**
     * Every timer's timeout this slot is called. It is used for that neat animations.
     *
     * @param event The event itself.
     */
    void timerEvent(QTimerEvent *event);

    /**
     * Resets the model.
     */
    void slotModelReset();
    /**
     * Layout of the modal changed.
     */
    void slotModelLayoutChanged();
    /**
     * Rows were inserted into the model.
     *
     * @param parent Parent item.
     * @param start Starting row.
     * @param end Ending row.
     */
    void slotModelRowsInserted(const QModelIndex &parent, int start, int end);
    /**
     * Rows were removed from the model.
     *
     * @param parent Parent item.
     * @param start Starting row.
     * @param end Ending row.
     */
    void slotModelRowsRemoved(const QModelIndex &parent, int start, int end);
    /**
     * Model data was changes.
     *
     * @param start The first item.
     * @param end The last item.
     */
    void slotModelDataChanged(const QModelIndex &start, const QModelIndex &end);
    /**
     * Slot for removing selected photos/items.
     */
    void slotRemove();

    /**
     * Connect the navigation buttons.
     */
    void slotConnectNavButtons();

    /**
     * Does the actual crop operation.
     *
     * @param area Area of operation.
     */
    void cropSelection(const QRect &area);
};

}

#endif
