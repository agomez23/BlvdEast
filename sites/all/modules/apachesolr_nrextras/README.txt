
This module follows node references on nodes being indexed.

It adds an anonymous view of the referenced node into its own Solr index field
belonging to the node being indexed. Then, during searches, it adds this new
field to the search, allowing the referring node to be discovered in search
with keywords and text belonging to the referred node.

The new field follows content_permissions module access rules from the original
CCK nodereference field.