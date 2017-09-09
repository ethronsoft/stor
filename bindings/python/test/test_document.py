import copy
from ethronsoft.pystor.document import *

def test_copy():
    doc = Document(Cstor.load(), "{}")
    deep = copy.deepcopy(doc)
    assert doc.id != deep.id

    doc.from_json(json.dumps([1, 2, 3]))
    assert doc.to_json() != deep.to_json()

def test_object():
    doc = Document(Cstor.load(), "{}")
    assert doc.type == NodeType.OBJECT
    assert doc is not None
    assert len(doc.id) > 0
    assert doc.to_json() == "{}"
    assert doc.value == {}

    obj = {"a": "hello", "b": {"z": [1L,2.0,True]}}

    doc.value = obj
    assert doc.value == obj
    assert json.loads(doc.to_json()) == obj

    assert len(doc) == 2
    assert len(doc["b"]) == 1
    assert len(doc["b"]["z"]) == 3
    assert 1 not in doc
    assert "a" in doc
    assert "b" in doc
    assert "z" not in doc
    assert "z" in doc["b"]
    assert 1 in doc["b"]["z"]
    assert 4 not in doc["b"]["z"]

    assert doc["a"].value == "hello"
    doc["a"] = "hello, world!"
    assert doc["a"].value == "hello, world!"
    assert doc["b"].value == obj["b"]
    assert doc["b"]["z"].value == obj["b"]["z"]

    keys = obj.keys()
    assert keys == doc.keys()
    for key in doc:
        assert key in doc


def test_array():
    doc = Document(Cstor.load(), "[]")
    assert doc is not None
    assert doc.type == NodeType.ARRAY
    assert len(doc.id) > 0
    assert doc.to_json() == "[]"
    assert doc.value == []

    arr = [1.0, 2L, "a", False, 5]

    doc.value = arr
    assert doc.value == arr
    assert json.loads(doc.to_json()) == arr

    assert len(doc) == len(arr)
    assert 1.0 in doc
    assert 4 not in doc
    assert "a" in doc
    assert "b" not in doc
    assert False in doc
    assert 2L in doc
    assert 5 in doc

    assert doc[0].value == 1.0
    assert doc[1].value == 2L
    assert doc[2].value == "a"
    assert doc[3].value is False
    assert doc[4].value == 5

    for item in doc:
        assert item.value in arr


def test_value():
    doc = Document(Cstor.load(), "null")
    assert doc.type == NodeType.NULL
    assert doc is not None
    assert len(doc.id) > 0

    doc.value = 1
    assert doc.type == NodeType.INT
    assert doc.value == 1

    doc.value = 1L
    assert doc.type == NodeType.LONG
    assert doc.value == 1L

    doc.value = 1.0
    assert doc.type == NodeType.FLOAT
    assert doc.value == 1.0

    doc.value = True
    assert doc.type == NodeType.BOOL
    assert doc.value is True

    doc.value = "hello"
    assert doc.type == NodeType.STRING
    assert doc.value == "hello"




