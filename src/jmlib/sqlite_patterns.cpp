/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 

#include "jmlib.h"
#include "sqlite_patterns.h"
#include "patterns.h"
#include "jugglesaver/js_patterns.h"

// JMPatternIterator
pattern_t* JMPatternIterator::operator[] (unsigned int i) {
  pattern_t* temp = m_first;
  
  while (i > 0 && temp) {
    i--;
    temp = temp->next;
  }

  m_current = temp;
  return m_current;
}

pattern_t* JMPatternIterator::first() {
  m_current = m_first;
  return m_current;
}

pattern_t* JMPatternIterator::last() {
  pattern_t* temp = m_first;
  
  while (temp->next) {
    temp = temp->next;
  }

  m_current = temp;
  return m_current;
}

pattern_t* JMPatternIterator::next() {
  if (m_current->next) m_current = m_current->next;
  return m_current;
}

pattern_t* JMPatternIterator::prev() {
  if (m_current->prev) m_current = m_current->prev;
  return m_current;
}

pattern_t* JMPatternIterator::random() {
  return NULL;
}

pattern_t* JMPatternIterator::nextCategory() {

  return m_current;
}

pattern_t* JMPatternIterator::prevCategory() {

  return m_current;
}

int JMPatternIterator::getCount() {
  return 1;
}

void JMPatternIterator::loadCurrent(JMLib* jm) {

}

JMCategoryIterator* JMPatternIterator::getCategoryIterator() {
  return NULL;
}

/*
 * pattern: name, data, style, author, hr, dr
 * style: name, data, length
 * camera placement: see SetCamera for variables
 *
 */

// create an empty sqlite database
void JMPatterns::createDB() {
  const char* sql = "DROP TABLE IF EXISTS CameraData;"
                    "CREATE TABLE CameraData (id PRIMARY KEY);"
                    "DROP TABLE IF EXISTS Category;"
                    "CREATE TABLE Category (name PRIMARY KEY);"
                    "DROP TABLE IF EXISTS Pattern;"
                    "CREATE TABLE Pattern (name PRIMARY KEY, data NOT NULL, category REFERENCES Category, style NOT NULL,"
                      "author, hr, dr, object_count, cameradata REFERENCES CameraData);"
                    "DROP TABLE IF EXISTS Style;"
                    "CREATE TABLE Style (name PRIMARY KEY, data NOT NULL, length NOT NULL);";
  
  const char* dbname = "test.db";
  char* zErr;
  int rc;

  rc = sqlite3_open(dbname, &db_);
  if(rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_));
    sqlite3_close(db_);
    return;
  }

  rc = sqlite3_exec(db_, sql, NULL, NULL, &zErr);
  if(rc != SQLITE_OK) {
    if (zErr != NULL) {
      fprintf(stderr, "SQL error: %s\n", zErr);
      sqlite3_free(zErr);
    }
  }
}

void JMPatterns::queryDB(const char* query) {
  char* zErr;
  int rc = sqlite3_exec(db_, query, NULL, NULL, &zErr);
  if(rc != SQLITE_OK) {
    if (zErr != NULL) {
      fprintf(stderr, "SQL error: %s\n", zErr);
      fprintf(stderr, "\tStatement: '%s'\n", query);
      sqlite3_free(zErr);
    }
  }
}

void JMPatterns::addCategory(pattern_group_t* group) {
  char* sql = sqlite3_mprintf("INSERT INTO Category VALUES (%Q)", group->name);
  queryDB(sql);  
  sqlite3_free(sql);
}

void JMPatterns::addPattern(pattern_t* patt, pattern_group_t* group) {
	int object_count = -1;

	if (strcmp(patt->style, "JuggleSaver") == 0) {
		//fixme: get object count of pattern for JuggleSaver
		// currently: the site for a JuggleSaver pattern is parsed in ParsePattern in engine.cpp
		// rewrite this to be non-desructive for GetCurrentSite
	}
	// for JuggleMaster, we already have the raw site
	else {
		JML_BOOL valid = JMSiteValidator::validateSSS(patt->data);
		if (valid) object_count = JMSiteValidator::getBallCount();
	}

  char* sql = sqlite3_mprintf("INSERT INTO Pattern VALUES (%Q, %Q, %Q, %Q, %Q, %f, %f, %d, NULL)",
          patt->name, patt->data, group->name, patt->style, patt->author, patt->hr, patt->dr, object_count);
  queryDB(sql);
  sqlite3_free(sql);
}

void JMPatterns::addStyle(style_t* style) {
  char* sql = sqlite3_mprintf("INSERT INTO Style VALUES (%Q, %Q, %d)", style->name, style->data, style->length);
  queryDB(sql);
  sqlite3_free(sql);
}

void JMPatterns::closeDB() {
  if (db_) sqlite3_close(db_);
}

// initializes an sql database from JuggleMaster and/or JuggleSaver pattern files
void JMPatterns::initializeDatabase(FILE* out, FILE* inJM, FILE* inJS) {
	groups_t groups;
  groups.first = NULL;
	styles_t styles;
  styles.first = NULL;
  int result = false;

  createDB();

	//if (!out) return;
	if (!inJM && !inJS) return;

	if (inJM && !inJS) {
		result = ParsePatterns(inJM, &groups, &styles);
  }
  else if (inJS && !inJM) {
    result = ParseJSPatterns(inJS, &groups);
  }
  else { // both
    result = ParseAllPatterns(inJM, inJS, &groups, &styles);
  }

	pattern_group_t* cur_group = groups.first;
	pattern_t* cur_pattern = NULL;

	while (cur_group) {
    addCategory(cur_group);
  
		cur_pattern = cur_group->first_patt;

		while (cur_pattern) {
      addPattern(cur_pattern, cur_group);
			cur_pattern = cur_pattern->next;
		}

		cur_group = cur_group->next;
	}

	style_t* cur_style = styles.first;

	while (cur_style) {
    addStyle(cur_style);  
		cur_style = cur_style->next;
	}
  
  /*
  // test
  pattern_t* patt = searchQuery("SELECT * FROM Pattern WHERE style LIKE 'JuggleSaver'");
  pattern_group_t* categories = getCategories();
  pattern_t* cat1 = getCategory(categories ? categories->name : NULL);
  categories = categories ? categories->next : NULL;
  pattern_t* cat2 = getCategory(categories ? categories->name : NULL);
  categories = categories ? categories->next : NULL;
  pattern_t* cat3 = getCategory(categories ? categories->name : NULL);
  categories = categories ? categories->next : NULL;
  */
}

pattern_t* JMPatterns::getCategory(const char* category) {
  char* sql = sqlite3_mprintf("SELECT * FROM Pattern WHERE category LIKE %Q", category);
  pattern_t* patt = searchQuery(sql);
  sqlite3_free(sql);
  return patt;  
}

pattern_group_t* JMPatterns::getCategories() {
  char* sql = "SELECT * FROM Category";
  pattern_group_t* first_group = NULL;
	pattern_group_t* new_group = NULL;
  pattern_group_t* cur_group = NULL;
  
  char** result; 
  int nrows, ncols;
  char* zErr;
  int rc = sqlite3_get_table(db_, sql, &result, &nrows, &ncols, &zErr);
  
  if(rc != SQLITE_OK) {
    if (zErr != NULL) {
      fprintf(stderr, "SQL error: %s\n", zErr);
      fprintf(stderr, "\tStatement: '%s'\n", sql);
      sqlite3_free(zErr);
      return NULL;
    }
  }

  if (nrows == 0) return NULL;

  for(int i=0; i < nrows; i++) {
    char* name   = result[(i+1)*ncols];
    
    new_group = new pattern_group_t;
    new_group->name = strdup(name);
    new_group->first_patt = NULL;
    new_group->next = NULL;
    new_group->prev = cur_group;
    
    if (!first_group) {
      first_group = new_group;
      cur_group = new_group;
    }
    else {
      cur_group->next = new_group;
      cur_group = new_group;
    }
  }

  sqlite3_free_table(result);
  sqlite3_free(zErr);
  return first_group;
}

pattern_t* JMPatterns::loadPattern(const pattern_t* patt, int offset, JMLib* jm) {
  pattern_t* temp = (pattern_t*)patt;
  
  while (offset > 0 && temp) {
    offset--;
    temp = temp->next;
  }

  jm->stopJuggle();
  jm->setPattern(temp->name, temp->data, temp->hr, temp->dr);
  jm->setStyleDefault();
  jm->startJuggle();
}

pattern_t* JMPatterns::loadNextPattern(const pattern_t* patt, JMLib* jm) {

}

pattern_t* JMPatterns::loadPrevPattern(const pattern_t* patt, JMLib* jm) {

}

pattern_t* JMPatterns::loadRandomPattern(JMLib* jm, const char** categories, bool exclude) {

}

pattern_t* JMPatterns::search(const char* item) {
	return searchQuery(NULL);
}

pattern_t* JMPatterns::search(const char* name, const char* site, const char* style, int minBalls, int maxBalls) {
	return searchQuery(NULL);
}

// Assumes a query in the Pattern table
pattern_t* JMPatterns::searchQuery(const char* query) {
	if (!query || query[0] == '\0') return NULL;

  pattern_t* first_patt = NULL;
	pattern_t* new_patt = NULL;
  pattern_t* cur_patt = NULL;
  
  char** result; 
  int nrows, ncols;
  char* zErr;
  int rc = sqlite3_get_table(db_, query, &result, &nrows, &ncols, &zErr);
  
  if(rc != SQLITE_OK) {
    if (zErr != NULL) {
      fprintf(stderr, "SQL error: %s\n", zErr);
      fprintf(stderr, "\tStatement: '%s'\n", query);
      sqlite3_free(zErr);
      return NULL;
    }
  }

  if (nrows == 0) return NULL;

  for(int i=0; i < nrows; i++) {
    char* name   = result[(i+1)*ncols + 0];
    char* data   = result[(i+1)*ncols + 1];
    char* cat    = result[(i+1)*ncols + 2];
    char* style  = result[(i+1)*ncols + 3];
    char* author = result[(i+1)*ncols + 4];
    char* hr_c   = result[(i+1)*ncols + 5];
    char* dr_c   = result[(i+1)*ncols + 6];
  
    new_patt = new pattern_t;
    new_patt->name = strdup(name);
    new_patt->data = strdup(data);
    new_patt->author = strdup(author);
    new_patt->style = strdup(style);
    new_patt->hr = atof(hr_c);
    new_patt->dr = atof(dr_c);
    new_patt->style = strdup(style);
    new_patt->ga = 0.0f; // ?
    new_patt->sp = 1.0f;
    new_patt->next = NULL;
    new_patt->prev = cur_patt;
    
    if (!first_patt) {
      first_patt = new_patt;
      cur_patt = new_patt;
    }
    else {
      cur_patt->next = new_patt;
      cur_patt = new_patt;
    }
  }
  
  sqlite3_free_table(result);
  sqlite3_free(zErr);

	return first_patt;
}


void JMPatterns::freeSearchResult(pattern_t* patt) {
	struct pattern_t *tmppatt = NULL;
	struct pattern_group_t *group = NULL;
	struct pattern_group_t *tmpgroup = NULL;

	while (patt) {
		if(patt->name != NULL) delete patt->name;
		if(patt->data != NULL) delete patt->data;
		if(patt->style != NULL) delete patt->style;
		if(patt->author != NULL) delete patt->author;
		tmppatt = patt;
		patt = patt->next;
		delete tmppatt;
	}
}

void JMPatterns::freeCategories(pattern_group_t* group) {
	struct pattern_group_t *tmpgroup = NULL;

	while (group) {
		if(group->name != NULL) delete group->name;
		tmpgroup = group;
		group = group->next;
		delete tmpgroup;
	}
}
