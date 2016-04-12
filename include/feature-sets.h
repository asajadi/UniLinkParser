/*********************************************************************************
*	Armin Sajadi, 2007
*	asajadi@gmail.com
*********************************************************************************/

Feature_connector * feature_conn_set_lookup(char * source, Feature_conn_set * fcs);
Feature_template * feature_template_set_lookup(char * source, Feature_template_set * fts);
Feature_conn_set * feature_conn_set_create(void);
Feature_template_set * feature_template_set_create(void);
void feature_template_set_delete(Feature_template_set *fts);
void feature_conn_set_delete(Feature_conn_set *fcs);
Feature_template * feature_template_set_add(Feature_template * source, Feature_template_set * fts);
Feature_connector * feature_conn_set_add(Feature_connector * source, Feature_conn_set * fcs);
