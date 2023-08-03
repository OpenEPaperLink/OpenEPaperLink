import json
import os
from json import JSONDecodeError
from typing import Callable, Any, List

"""
a stupid, trivial and probably suboptimal database implementation,
that we use to preserve some runtime-information about tags
"""


class TagDb:
    def __init__(self):
        self.tags = {}
        self.listeners: List[Callable[[dict], Any]] = []

    def get_tag(self, mac):
        if mac not in self.tags:
            self.tags[mac] = {}
        return self.tags[mac]

    def save(self):
        os.makedirs('cache', exist_ok=True)
        with open('cache/tagdb.json', 'wt') as f:
            json.dump(self.tags, f, indent=4)

    def load(self):
        if os.path.exists("cache/tagdb.json"):
            try:
                with open("cache/tagdb.json", 'rt') as f:
                    self.tags = json.load(f)
                    for listener in self.listeners:
                        listener(self.tags)
            except JSONDecodeError:
                print("tagdb.json is invalid. ignoring")

    def notify_change(self, tag_key):
        for listener in self.listeners:
            listener({tag_key: self.tags[tag_key]})

    def on_update(self, listener: Callable[[dict], Any]):
        self.listeners.append(listener)
