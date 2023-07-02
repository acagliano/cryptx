#!/usr/bin/env python3
import sys

sys.path.append('/opt/homebrew/lib/python3.11/site-packages')
import subprocess, os

project = 'CryptX'
copyright = '2023'
author = 'Anthony Cagliano'

current_version = os.environ['current_version']
default_version = os.environ['default_version']
versions = os.environ['versions'].split()

extensions = [
    'breathe',
]

html_theme_options = {
    "globaltoc_collapse": False,
}

import guzzle_sphinx_theme

html_theme_path = guzzle_sphinx_theme.html_theme_path()
html_theme = 'guzzle_sphinx_theme'

# Register the theme as an extension to generate a sitemap.xml
extensions.append("guzzle_sphinx_theme")


latex_engine = 'pdflatex'
latex_elements = {
  'papersize': 'letterpaper',
  'figure_align': 'H',
  'extraclassoptions': 'openany,oneside',
  'fncychap': r'\usepackage[Sonny]{fncychap}',
  'pointsize': '11pt',
  'preamble': r'''
    \usepackage{charter}
    \usepackage[defaultsans]{lato}
    \usepackage{inconsolata}
  ''',
}

templates_path = ['templates']
source_suffix = '.rst'
master_doc = 'index'
language = None
exclude_patterns = ['build', '_build', 'Thumbs.db', '.DS_Store', 'venv']
pygments_style = None
html_theme = 'guzzle_sphinx_theme'
html_show_sourcelink = False

try:
   html_context
except NameError:
   html_context = dict()
html_context['display_lower_left'] = True
html_context['current_version'] = current_version
html_context['default_version'] = default_version
html_context['version'] = current_version
html_context['versions'] = list()
for version in versions:
   html_context['versions'].append( (version, '/toolchain/' + version + '/index.html') )

breathe_projects = {}
breathe_default_project = 'CE C/C++ Toolchain'
breathe_show_define_initializer = True
breathe_show_enumvalue_initializer = True
subprocess.call('doxygen doxyfile', shell=True)
breathe_projects['CE C/C++ Toolchain'] = 'doxygen/xml'
breathe_projects['CE C Toolchain'] = 'doxygen/xml'
