/*
 * Copyright (C) 2011-2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TileController_h
#define TileController_h

#include "FloatRect.h"
#include "IntRect.h"
#include "PlatformCALayer.h"
#include "PlatformCALayerClient.h"
#include "TiledBacking.h"
#include "Timer.h"
#include <wtf/Deque.h>
#include <wtf/HashMap.h>
#include <wtf/Noncopyable.h>
#include <wtf/RetainPtr.h>

namespace WebCore {

class FloatRect;
class IntPoint;
class IntRect;
class TileCoverageMap;
class TileGrid;

typedef Vector<RetainPtr<PlatformLayer>> PlatformLayerList;

class TileController final : public TiledBacking {
    WTF_MAKE_NONCOPYABLE(TileController); WTF_MAKE_FAST_ALLOCATED;
    friend class TileCoverageMap;
    friend class TileGrid;
public:
    WEBCORE_EXPORT explicit TileController(PlatformCALayer*);
    ~TileController();
    
    WEBCORE_EXPORT static String tileGridContainerLayerName();
    static String zoomedOutTileGridContainerLayerName();

    WEBCORE_EXPORT void tileCacheLayerBoundsChanged();

    WEBCORE_EXPORT void setNeedsDisplay();
    WEBCORE_EXPORT void setNeedsDisplayInRect(const IntRect&);

    WEBCORE_EXPORT void setContentsScale(float);
    WEBCORE_EXPORT float contentsScale() const;

    bool acceleratesDrawing() const { return m_acceleratesDrawing; }
    WEBCORE_EXPORT void setAcceleratesDrawing(bool);

    WEBCORE_EXPORT void setTilesOpaque(bool);
    bool tilesAreOpaque() const { return m_tilesAreOpaque; }

    PlatformCALayer& rootLayer() { return *m_tileCacheLayer; }
    const PlatformCALayer& rootLayer() const { return *m_tileCacheLayer; }

    WEBCORE_EXPORT void setTileDebugBorderWidth(float);
    WEBCORE_EXPORT void setTileDebugBorderColor(Color);

    FloatRect visibleRect() const override { return m_visibleRect; }
    FloatRect coverageRect() const override { return m_coverageRect; }

    unsigned blankPixelCount() const;
    static unsigned blankPixelCountForTiles(const PlatformLayerList&, const FloatRect&, const IntPoint&);

#if PLATFORM(IOS)
    unsigned numberOfUnparentedTiles() const;
    void removeUnparentedTilesNow();
#endif

    float deviceScaleFactor() const { return m_deviceScaleFactor; }

    Color tileDebugBorderColor() const { return m_tileDebugBorderColor; }
    float tileDebugBorderWidth() const { return m_tileDebugBorderWidth; }
    ScrollingModeIndication indicatorMode() const { return m_indicatorMode; }

    IntSize tileSize() const override;
    IntRect bounds() const override;
    IntRect boundsWithoutMargin() const override;
    bool hasMargins() const override;
    bool hasHorizontalMargins() const override;
    bool hasVerticalMargins() const override;
    int topMarginHeight() const override;
    int bottomMarginHeight() const override;
    int leftMarginWidth() const override;
    int rightMarginWidth() const override;
    TileCoverage tileCoverage() const override { return m_tileCoverage; }
    void adjustTileCoverageRect(FloatRect& coverageRect, const FloatSize& newSize, const FloatRect& previousVisibleRect, const FloatRect& currentVisibleRect, float contentsScale) const override;
    bool unparentsOffscreenTiles() const override { return m_unparentsOffscreenTiles; }
    bool scrollingPerformanceLoggingEnabled() const override { return m_scrollingPerformanceLoggingEnabled; }

    IntRect boundsAtLastRevalidate() const { return m_boundsAtLastRevalidate; }
    IntRect boundsAtLastRevalidateWithoutMargin() const;
    void didRevalidateTiles();

    bool shouldAggressivelyRetainTiles() const;
    bool shouldTemporarilyRetainTileCohorts() const;

    void updateTileCoverageMap();

    RefPtr<PlatformCALayer> createTileLayer(const IntRect&, TileGrid&);

    const TileGrid& tileGrid() const { return *m_tileGrid; }

    WEBCORE_EXPORT Vector<RefPtr<PlatformCALayer>> containerLayers();

private:
    TileGrid& tileGrid() { return *m_tileGrid; }

    void scheduleTileRevalidation(double interval);

    bool isInWindow() const { return m_isInWindow; }
    float topContentInset() const { return m_topContentInset; }

    // TiledBacking member functions.
    void setVisibleRect(const FloatRect&) override;
    void setCoverageRect(const FloatRect&) override;
    bool tilesWouldChangeForCoverageRect(const FloatRect&) const override;
    void setTiledScrollingIndicatorPosition(const FloatPoint&) override;
    void setTopContentInset(float) override;
    void setVelocity(const VelocityData&) override;
    void prepopulateRect(const FloatRect&) override;
    void setIsInWindow(bool) override;
    void setTileCoverage(TileCoverage) override;
    void revalidateTiles() override;
    void forceRepaint() override;
    IntRect tileGridExtent() const override;
    void setScrollingPerformanceLoggingEnabled(bool flag) override { m_scrollingPerformanceLoggingEnabled = flag; }
    void setUnparentsOffscreenTiles(bool flag) override { m_unparentsOffscreenTiles = flag; }
    double retainedTileBackingStoreMemory() const override;
    IntRect tileCoverageRect() const override;
#if USE(CA)
    PlatformCALayer* tiledScrollingIndicatorLayer() override;
#endif
    void setScrollingModeIndication(ScrollingModeIndication) override;
    void setTileMargins(int marginTop, int marginBottom, int marginLeft, int marginRight) override;
    void setZoomedOutContentsScale(float) override;
    float zoomedOutContentsScale() const override;

    void clearZoomedOutTileGrid();
    void tileGridsChanged();

    void tileRevalidationTimerFired();
    void setNeedsRevalidateTiles();
    
    IntRect boundsForSize(const FloatSize&) const;

    PlatformCALayerClient* owningGraphicsLayer() const { return m_tileCacheLayer->owner(); }

    PlatformCALayer* m_tileCacheLayer;

    std::unique_ptr<TileCoverageMap> m_coverageMap;

    std::unique_ptr<TileGrid> m_tileGrid;
    std::unique_ptr<TileGrid> m_zoomedOutTileGrid;

    FloatRect m_visibleRect; // Only used for scroll performance logging.
    FloatRect m_coverageRect;
    IntRect m_boundsAtLastRevalidate;

    Timer m_tileRevalidationTimer;

    float m_zoomedOutContentsScale;
    float m_deviceScaleFactor;

    TileCoverage m_tileCoverage;
    
    VelocityData m_velocity;

    // m_marginTop and m_marginBottom are the height in pixels of the top and bottom margin tiles. The width
    // of those tiles will be equivalent to the width of the other tiles in the grid. m_marginRight and
    // m_marginLeft are the width in pixels of the right and left margin tiles, respectively. The height of
    // those tiles will be equivalent to the height of the other tiles in the grid.
    int m_marginTop;
    int m_marginBottom;
    int m_marginLeft;
    int m_marginRight;

    bool m_isInWindow;
    bool m_scrollingPerformanceLoggingEnabled;
    bool m_unparentsOffscreenTiles;
    bool m_acceleratesDrawing;
    bool m_tilesAreOpaque;
    bool m_hasTilesWithTemporaryScaleFactor; // Used to make low-res tiles when zooming.

    Color m_tileDebugBorderColor;
    float m_tileDebugBorderWidth;
    ScrollingModeIndication m_indicatorMode;
    float m_topContentInset;
};

} // namespace WebCore

#endif // TileController_h
