
inline_registration.module

-> THIS MODULE IS STILL IN EARLY DEVELOPMENT AND
-> IT IS INADVISABLE TO USE IT ON A PRODUCTION SITE.

Inline Registration allows anonymous users to register via the node/add page, thus removing a step/barrier from the user actually publishing content. If you're going to use this module, or allow anonymous users to post content to your site at all for that matter, you should really use CAPTCHA to help keep the spam-bots from trashing your site.

FEATURES:
- Adds user_register() form to node/add pages if the user is not logged in
- Associates the new piece of content with the new user
- Can log the user in after node creation depending on user registration settings

INSTALLATION:
- Copy the inline_registration directory into your site's modules directory.
- Enable the module at admin/build/modules.
- Enable the create content permission for anonymous users for each node type anonymous users should be able to create
- Decide on what nodes you want to allow anonymous users to post content to. Then go to /admin/content/types and edit those node types that you selected. Do this by scrolling to the bottom of the content type page where you find the Inline Registration section. Check the box and set the weight where this is shown in your node type. Save the settings.