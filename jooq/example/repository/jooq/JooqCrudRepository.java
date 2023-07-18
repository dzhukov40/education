package com.engpie.eptrunkimpl.repository.jooq;

import java.util.List;
import java.util.Optional;

/**
 * TODO: add description.
 *
 * @author : imba
 **/
public interface JooqCrudRepository<T, ID> extends JooqRepository<T, ID> {

  long count();

  void delete(T entity);

  void deleteAll();

  void deleteAll(List<? extends T> entities);

  void deleteAllById(List<? extends ID> ids);

  void deleteById(ID id);

  boolean existsById(ID id);

  List<T> findAll();

  List<T> findAllById(List<ID> ids);

  Optional<T> findById(ID id);

  T save(T entity);

  List<T> saveAll(List<T> entities);

}
