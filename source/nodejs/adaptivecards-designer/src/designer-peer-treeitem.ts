// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
import { Carousel } from "adaptivecards";
import { BaseTreeItem } from "./base-tree-item";
import { CarouselPagePeer, CarouselPeer, DesignerPeer } from "./designer-peers";

export class DesignerPeerTreeItem extends BaseTreeItem {
    private computeLevel() {
        this._level = 0;

        let peer = this.owner;

        while (peer) {
            this._level++;

            peer = peer.parent;
        }

        for (let i = 0; i < this.getChildCount(); i++) {
            (this.getChildAt(i) as DesignerPeerTreeItem).computeLevel();
        }
    }

    protected getIconClass(): string {
        return this.owner.registration.iconClass;
    }

    protected getLabelText(): string {
        return this.owner.getCardObject().getJsonTypeName();
    }

    protected getAdditionalText(): string {
        return this.owner.getTreeItemText();
    }

    protected selectedChanged(scrollIntoView: boolean) {
        super.selectedChanged(scrollIntoView);

        this.owner.isSelected = this.isSelected;
    }

    readonly owner: DesignerPeer;

    constructor(owner: DesignerPeer, isExpanded: boolean = true) {
        super(isExpanded);

        this.owner = owner;
        this.owner.onParentChanged = (sender: DesignerPeer) => {
            this.computeLevel();
        }

        this.computeLevel();
    }

    getChildCount(): number {
        return this.owner.getChildCount();
    }

    getChildAt(index: number): BaseTreeItem {
        return this.owner.getChildAt(index).treeItem;
    }

    protected click(e: MouseEvent) {
        super.click(e);

        let currentPeer = this.owner;

        while (currentPeer) {
            if (currentPeer instanceof CarouselPagePeer) {

                const carouselPeer = currentPeer.parent as CarouselPeer;
                if (carouselPeer) {

                    const carouselElement = (carouselPeer.cardElement as Carousel);
                    if (carouselElement && carouselElement.carousel) {
                        const index = carouselPeer.children.indexOf(currentPeer) + (carouselElement.carousel.loopedSlides || 0);
                        carouselElement.carousel.slideTo(index);
                    }
                }
                
                break;
            }
            currentPeer = currentPeer.parent;
        }
    }
}
